package main

import (
	"encoding/json"
	"fmt"

	"github.com/incognitochain/incognito-chain/blockchain"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/incognitokey"
	"github.com/incognitochain/incognito-chain/privacy"
	"github.com/syndtr/goleveldb/leveldb"
)

var localnode interface {
	GetUserDatabase() *leveldb.DB
	GetBlockchain() *blockchain.BlockChain
}
var CoinProcessedState map[byte]uint64

func OnNewShardBlock(bc *blockchain.BlockChain, h common.Hash, height uint64) {
	var blk blockchain.ShardBlock
	blkBytes, err := localnode.GetUserDatabase().Get(h.Bytes(), nil)
	if err != nil {
		fmt.Println(err)
		return
	}
	if err := json.Unmarshal(blkBytes, &blk); err != nil {
		fmt.Println(err)
		return
	}
	transactionStateDB := bc.GetBestStateShard(byte(blk.GetShardID())).GetCopiedTransactionStateDB()
	if len(blk.Body.Transactions) > 0 {
		err = bc.CreateAndSaveTxViewPointFromBlock(&blk, transactionStateDB)
		if err != nil {
			panic(err)
		}
	}
	transactionRootHash, err := transactionStateDB.Commit(true)
	if err != nil {
		panic(err)
	}
	err = transactionStateDB.Database().TrieDB().Commit(transactionRootHash, false)
	if err != nil {
		panic(err)
	}
	bc.GetBestStateShard(byte(blk.GetShardID())).TransactionStateDBRootHash = transactionRootHash
	batchData := bc.GetShardChainDatabase(blk.Header.ShardID).NewBatch()
	err = bc.BackupShardViews(batchData, blk.Header.ShardID)
	if err != nil {
		panic("Backup shard view error")
	}

	if err := batchData.Write(); err != nil {
		panic(err)
	}
	statePrefix := fmt.Sprintf("coin-processed-%v", blk.Header.ShardID)
	err = localnode.GetUserDatabase().Put([]byte(statePrefix), []byte(fmt.Sprintf("%v", blk.Header.Height)), nil)
	if err != nil {
		panic(err)
	}
	CoinProcessedState[blk.Header.ShardID] = blk.Header.Height
}

func GetCoinsByPrivateKey(keyset *incognitokey.KeySet, tokenID *common.Hash) ([]*privacy.OutputCoin, error) {
	lastByte := keyset.PaymentAddress.Pk[len(keyset.PaymentAddress.Pk)-1]
	shardIDSender := common.GetShardIDFromLastByte(lastByte)
	if tokenID == nil {
		tokenID = &common.Hash{}
		tokenID.SetBytes(common.PRVCoinID[:])
	}
	outcoinList, err := localnode.GetBlockchain().GetListOutputCoinsByKeyset(keyset, shardIDSender, tokenID)
	if err != nil {
		return nil, err
	}
	amount := uint64(0)
	for _, out := range outcoinList {
		amount += out.CoinDetails.GetValue()
	}
	fmt.Println("amount", amount)
	fmt.Println("len(outcoinList)", len(outcoinList))
	return outcoinList, nil
}

func GetCoinsByPaymentAddress(keyset *incognitokey.KeySet, tokenID *common.Hash) ([]*privacy.OutputCoin, error) {
	lastByte := keyset.PaymentAddress.Pk[len(keyset.PaymentAddress.Pk)-1]
	shardIDSender := common.GetShardIDFromLastByte(lastByte)
	if tokenID == nil {
		tokenID = &common.Hash{}
		tokenID.SetBytes(common.PRVCoinID[:])
	}
	outcoinList, err := localnode.GetBlockchain().GetListOutputCoinsByKeyset(keyset, shardIDSender, tokenID)
	if err != nil {
		return nil, err
	}
	fmt.Println("len(outcoinList)", len(outcoinList))
	return outcoinList, nil
}
