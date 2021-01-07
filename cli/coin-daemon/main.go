package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"time"

	devframework "github.com/0xkumi/incognito-dev-framework"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/privacy/coin"
	"github.com/incognitochain/incognito-chain/rpcserver/jsonresult"
	"github.com/incognitochain/incognito-chain/wallet"
)

const (
	MODELIGHT = "light"
	MODERPC   = "rpc"
	MODESIM   = "sim"
)

func main() {
	modeFlag := flag.String("mode", "light", "daemon mode")
	flag.Parse()
	switch *modeFlag {
	case MODELIGHT:
		node := devframework.NewAppNode("fullnode", devframework.TestNet2Param, true, false)
		localnode = node
		InitCoinsModule()
	case MODERPC:
		node := devframework.NewRPCClient("")
		_ = node
	case MODESIM:
		node := devframework.NewStandaloneSimulation("simnode", devframework.Config{
			ChainParam: devframework.NewChainParam(devframework.ID_TESTNET2).SetActiveShardNumber(2),
			DisableLog: true,
		})
		node.GenerateBlock().NextRound()
		node.ShowBalance(node.GenesisAccount)
		acc1 := node.NewAccountFromShard(0)
		acc2 := node.NewAccountFromShard(0)
		fmt.Println(acc1.PrivateKey)
		fmt.Println(acc2.PrivateKey)
		node.ShowBalance(acc1)
		node.ShowBalance(acc2)

		node.SendPRV(node.GenesisAccount, acc1, 1000, acc2, 1000)
		for i := 0; i < 10; i++ {
			node.GenerateBlock().NextRound()
		}

		l, err := node.RPC.API_ListOutputCoins(acc1.PrivateKey)
		if err != nil {
			panic(err)
		}
		// fmt.Println(l.Outputs)
		node.ShowBalance(acc1)
		node.ShowBalance(acc2)
		for s, out := range l.Outputs {
			fmt.Println("outs ", s, len(out))
			for _, c := range out {
				cBytes, _ := json.Marshal(c)
				fmt.Println(string(cBytes))
				cV2, err := jsonresult.NewCoinFromJsonOutCoin(c)
				if err != nil {
					panic(err)
				}
				cv2 := cV2.(*coin.CoinV2)
				cpl, err := cv2.Decrypt(acc1.Keyset)
				if err != nil {
					panic(err)
				}
				fmt.Println(string(cpl.GetKeyImage().MarshalText()))
			}
		}
		select {}
		// node.ApplyChain(0).GenerateBlock().NextRound()
		return
	default:
		panic("unknown mode")
	}

	wl, _ := wallet.Base58CheckDeserialize("112t8rnX5E2Mkqywuid4r4Nb2XTeLu3NJda43cuUM1ck2brpHrufi4Vi42EGybFhzfmouNbej81YJVoWewJqbR4rPhq2H945BXCLS2aDLBTA")
	for {
		coins, err := localnode.GetBlockchain().TryGetAllOutputCoinsByKeyset(&wl.KeySet, 0, &common.PRVCoinID, true)
		if err != nil {
			fmt.Println("sdfsdfDSF", err)
		}
		balance := uint64(0)
		for _, out := range coins {
			balance += out.GetValue()
		}
		fmt.Println("balance", balance)
		time.Sleep(5 * time.Second)
	}
	select {}
}
