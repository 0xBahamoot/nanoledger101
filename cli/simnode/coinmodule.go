package main

import (
	"fmt"

	"github.com/incognitochain/incognito-chain/blockchain"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/syndtr/goleveldb/leveldb"
)

var localnode interface {
	GetUserDatabase() *leveldb.DB
	GetBlockchain() *blockchain.BlockChain
}

func OnNewShardBlock(bc *blockchain.BlockChain, h common.Hash, height uint64) {
	blk, _, err := bc.GetShardBlockByHash(h)
	if err != nil {
		panic(err)
	}
	fmt.Println("Shard", blk.GetShardID(), blk.GetHeight())
	// if len(blk.Body.Transactions) > 0 {
	// 	err = bc.CreateAndSaveTxViewPointFromBlock(blk, bc.GetBestStateShard(byte(blk.GetShardID())).GetCopiedTransactionStateDB())
	// 	if err != nil {
	// 		panic(err)
	// 	}
	// }
}

func GetCoinsByPrivateKey() {

}

func GetCoinsByViewKey() {

}
