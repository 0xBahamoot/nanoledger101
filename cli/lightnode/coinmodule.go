package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"strconv"

	"github.com/incognitochain/incognito-chain/blockchain"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/dataaccessobject/statedb"
	"github.com/incognitochain/incognito-chain/incognitokey"
	"github.com/incognitochain/incognito-chain/multiview"
	"github.com/incognitochain/incognito-chain/privacy"
	"github.com/incognitochain/incognito-chain/wallet"
	"github.com/syndtr/goleveldb/leveldb"
)

var localnode interface {
	GetUserDatabase() *leveldb.DB
	GetBlockchain() *blockchain.BlockChain
	OnNewBlockFromParticularHeight(chainID int, blkHeight int64, isFinalized bool, f func(bc *blockchain.BlockChain, h common.Hash, height uint64))
}
var CoinProcessedState map[byte]uint64
var TransactionStateDB map[byte]*statedb.StateDB

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

	transactionStateDB := TransactionStateDB[byte(blk.GetShardID())]

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
	if (blk.Header.Height % 100) == 0 {
		// fmt.Println("RestoreShardViews")
		shardID := blk.Header.ShardID
		localnode.GetBlockchain().ShardChain[shardID] = blockchain.NewShardChain(int(shardID), multiview.NewMultiView(), localnode.GetBlockchain().GetConfig().BlockGen, localnode.GetBlockchain(), common.GetShardChainKey(shardID))
		if err := localnode.GetBlockchain().RestoreShardViews(shardID); err != nil {
			panic(err)
		}
		// TransactionStateDB[byte(blk.GetShardID())] = localnode.GetBlockchain().GetBestStateShard(blk.Header.ShardID).GetCopiedTransactionStateDB()
	}
}

// func GetCoinsByPrivateKey(keyset *incognitokey.KeySet, tokenID *common.Hash) ([]*privacy.OutputCoin, error) {
// 	lastByte := keyset.PaymentAddress.Pk[len(keyset.PaymentAddress.Pk)-1]
// 	shardIDSender := common.GetShardIDFromLastByte(lastByte)
// 	if tokenID == nil {
// 		tokenID = &common.Hash{}
// 		tokenID.SetBytes(common.PRVCoinID[:])
// 	}
// 	outcoinList, err := localnode.GetBlockchain().GetListOutputCoinsByKeyset(keyset, shardIDSender, tokenID)
// 	if err != nil {
// 		return nil, err
// 	}
// 	amount := uint64(0)
// 	for _, out := range outcoinList {
// 		amount += out.CoinDetails.GetValue()
// 	}
// 	fmt.Println("amount", amount)
// 	fmt.Println("len(outcoinList)", len(outcoinList))
// 	return outcoinList, nil
// }

func GetCoins(keyset *incognitokey.KeySet, tokenID *common.Hash) ([]*privacy.OutputCoin, error) {
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

func InitCoinsModule() {
	CoinProcessedState = make(map[byte]uint64)
	TransactionStateDB = make(map[byte]*statedb.StateDB)
	//load CoinProcessedState
	for i := 0; i < localnode.GetBlockchain().GetChainParams().ActiveShards; i++ {
		statePrefix := fmt.Sprintf("coin-processed-%v", i)
		v, err := localnode.GetUserDatabase().Get([]byte(statePrefix), nil)
		if err != nil {
			fmt.Println(err)
		}
		if v != nil {
			height, err := strconv.ParseUint(string(v), 0, 64)
			if err != nil {
				fmt.Println(err)
				continue
			}
			CoinProcessedState[byte(i)] = height
		} else {
			CoinProcessedState[byte(i)] = 1
		}
		TransactionStateDB[byte(i)] = localnode.GetBlockchain().GetBestStateShard(byte(i)).GetCopiedTransactionStateDB()
		fmt.Println("TransactionStateDB[byte(i)]", byte(i), TransactionStateDB[byte(i)])
	}
	for i := 0; i < localnode.GetBlockchain().GetChainParams().ActiveShards; i++ {
		localnode.OnNewBlockFromParticularHeight(i, int64(CoinProcessedState[byte(i)]), true, OnNewShardBlock)
	}
	go startService()
}

func startService() {
	http.HandleFunc("/getbalance", getBalanceHandler)
	http.HandleFunc("/getcoins", getCoinsHandler)
	err := http.ListenAndServe("127.0.0.1:9000", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}

func getCoinsHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	if r.Method != "GET" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	key := r.URL.Query().Get("key")
	wl, err := wallet.Base58CheckDeserialize(key)
	if err != nil {
		http.Error(w, "Unexpected error", http.StatusInternalServerError)
		return
	}
	outcoins, err := GetCoins(&wl.KeySet, nil)
	if err != nil {
		http.Error(w, "Unexpected error", http.StatusInternalServerError)
		return
	}
	coinsBytes, err := json.Marshal(outcoins)
	if err != nil {
		http.Error(w, "Unexpected error", http.StatusInternalServerError)
		return
	}
	w.WriteHeader(200)
	_, err = w.Write(coinsBytes)
	if err != nil {
		panic(err)
	}
	return
}

func getBalanceHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	if r.Method != "GET" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	w.WriteHeader(200)
	// _, err = w.Write(sysBytes)
	// if err != nil {
	// 	panic(err)
	// }
	return
}
