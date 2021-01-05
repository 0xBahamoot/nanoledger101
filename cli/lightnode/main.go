package main

import (
	"fmt"
	"time"

	devframework "github.com/0xkumi/incognito-dev-framework"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/wallet"
)

func main() {
	node := devframework.NewAppNode("fullnode", devframework.TestNet2Param, true, false)
	localnode = node
	InitCoinsModule()

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
