package main

import (
	"fmt"

	Inc "github.com/0xkumi/incognito-dev-framework"
	"github.com/incognitochain/incognito-chain/blockchain"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/syndtr/goleveldb/leveldb"
)

func main() {
	node := Inc.NewStandaloneSimulation("newsim", Inc.Config{
		ChainParam: Inc.NewChainParam(Inc.ID_TESTNET2).SetActiveShardNumber(2),
		DisableLog: true,
	})
	var OnNewShardBlock = func(bc *blockchain.BlockChain, h common.Hash, height uint64) {
		blk, _, err := bc.GetShardBlockByHash(h)
		if err != nil {
			panic(err)
		}
		fmt.Println("Shard", blk.GetShardID(), blk.GetHeight())
		if len(blk.Body.Transactions) > 0 {
			outcoinCount := 0
			batch := new(leveldb.Batch)
			for _, tx := range blk.Body.Transactions {
				for _, coin := range tx.GetProof().GetOutputCoins() {
					outcoinCount++
					b := coin.Bytes()
					batch.Put(coin.CoinDetails.HashH().Bytes(), b)
				}
			}
			err = node.GetUserDatabase().Write(batch, nil)
			if err != nil {
				panic(err)
			}
			fmt.Println("Saved", outcoinCount, "outcoins.")
		}
	}
	node.OnNewBlockFromParticularHeight(0, -1, true, OnNewShardBlock)

	node.GenerateBlock().NextRound()

	acc1 := node.NewAccountFromShard(0)
	acc2 := node.NewAccountFromShard(0)
	node.ShowBalance(acc1)
	node.ShowBalance(acc2)
	fmt.Println(acc1.PrivateKey)
	// wl, err := wallet.Base58CheckDeserialize(privateKey)
	// if err != nil {
	// 	return Account{}, err
	// }
	node.SendPRV(node.GenesisAccount, acc1, 1000, acc2, 1000)
	for i := 0; i < 10; i++ {
		node.GenerateBlock().NextRound()
	}

	node.ShowBalance(acc1)
	node.ShowBalance(acc2)

	node.ApplyChain(0).GenerateBlock().NextRound()

}
