package main

import (
	"encoding/hex"
	"encoding/json"
	"flag"
	"fmt"

	devframework "github.com/0xkumi/incognito-dev-framework"
	"github.com/0xkumi/incognito-dev-framework/account"
	"github.com/incognitochain/incognito-chain/privacy/coin"
	"github.com/incognitochain/incognito-chain/rpcserver/jsonresult"
)

const (
	MODELIGHT = "light"
	MODERPC   = "rpc"
	MODESIM   = "sim"
)

func main() {
	modeFlag := flag.String("mode", "light", "daemon mode")
	rpcFlag := flag.String("rpchost", "127.0.0.1:9334", "rpc host")
	flag.Parse()
	switch *modeFlag {
	case MODELIGHT:
		node := devframework.NewAppNode("fullnode", devframework.TestNet2Param, true, false)
		localnode = node
		initCoinService()
		initAccountService()
	case MODERPC:
		node := devframework.NewRPCClient(*rpcFlag)
		_ = node
	case MODESIM:
		node := devframework.NewStandaloneSimulation("simnode", devframework.Config{
			ChainParam: devframework.NewChainParam(devframework.ID_TESTNET2).SetActiveShardNumber(8),
			DisableLog: true,
		})
		node.GenerateBlock().NextRound()
		node.ShowBalance(node.GenesisAccount)
		acc0, _ := account.NewAccountFromPrivatekey("111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnPDGEQMGvA1pRfT71Crr7MM2ShvpkxCBWBL2icG22cXSpB8A2XKuezTJ")
		node.Pause()
		node.SendPRV(node.GenesisAccount, acc0, 10000)
		for i := 0; i < 10; i++ {
			node.GenerateBlock().NextRound()
		}

		l, err := node.RPC.API_ListOutputCoins(acc0.PrivateKey)
		if err != nil {
			panic(err)
		}

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
				cv2Bytes := cv2.Bytes()
				fmt.Println(hex.EncodeToString(cv2Bytes))

				coinDecrypted, err := cv2.Decrypt(acc0.Keyset)
				if err != nil {
					panic(err)
				}
				// fmt.Println(string(coinDecrypted.GetKeyImage().MarshalText()))
				coinDBytes, err := coinDecrypted.MarshalJSON()
				if err != nil {
					panic(err)
				}
				fmt.Println(coinDBytes)
			}
		}
		// node.ApplyChain(0).GenerateBlock().NextRound()
		return
	default:
		panic("unknown mode")
	}
	select {}
}
