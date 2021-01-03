package main

import (
	"fmt"

	Inc "github.com/0xkumi/incognito-dev-framework"
	"github.com/incognitochain/incognito-chain/common"
)

func main() {
	node := Inc.NewStandaloneSimulation("newsim", Inc.Config{
		ChainParam: Inc.NewChainParam(Inc.ID_TESTNET2).SetActiveShardNumber(2),
		DisableLog: true,
	})
	localnode = node
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
	if _, err := node.SendPRV(node.GenesisAccount, acc1, 1000, acc2, 1000); err != nil {
		panic(err)
	}
	for i := 0; i < 10; i++ {
		node.GenerateBlock().NextRound()
	}
	if _, err := node.SendPRV(node.GenesisAccount, acc1, 1000, acc2, 1000); err != nil {
		panic(err)
	}

	for i := 0; i < 10; i++ {
		node.GenerateBlock().NextRound()
	}
	node.ShowBalance(acc1)
	node.ShowBalance(acc2)

	lastByte := acc1.Keyset.PaymentAddress.Pk[len(acc1.Keyset.PaymentAddress.Pk)-1]
	shardIDSender := common.GetShardIDFromLastByte(lastByte)
	prvCoinID := &common.Hash{}
	prvCoinID.SetBytes(common.PRVCoinID[:])

	outcoinList, err := node.GetBlockchain().GetListOutputCoinsByKeyset(acc1.Keyset, shardIDSender, prvCoinID)
	if err != nil {
		panic(err)
	}
	fmt.Println("len(outcoinList)", len(outcoinList))
	if len(outcoinList) > 0 {
		panic(len(outcoinList))
	}

	node.ApplyChain(0).GenerateBlock().NextRound()
}
