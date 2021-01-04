package main

import (
	"fmt"
	"strconv"
	"time"

	devframework "github.com/0xkumi/incognito-dev-framework"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/dataaccessobject/statedb"
	"github.com/incognitochain/incognito-chain/wallet"
)

func main() {
	node := devframework.NewAppNode("fullnode", devframework.TestNet2Param, true, false)
	localnode = node
	CoinProcessedState = make(map[byte]uint64)
	TransactionStateDB = make(map[byte]*statedb.StateDB)
	//load CoinProcessedState
	for i := 0; i < 8; i++ {
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
	node.OnNewBlockFromParticularHeight(0, int64(CoinProcessedState[0]), true, OnNewShardBlock)
	wl, _ := wallet.Base58CheckDeserialize("112t8rnX5E2Mkqywuid4r4Nb2XTeLu3NJda43cuUM1ck2brpHrufi4Vi42EGybFhzfmouNbej81YJVoWewJqbR4rPhq2H945BXCLS2aDLBTA")
	for {
		coins, err := localnode.GetBlockchain().GetListOutputCoinsByKeyset(&wl.KeySet, 0, &common.PRVCoinID)
		if err != nil {
			fmt.Println(err)
		}
		balance := uint64(0)
		for _, out := range coins {
			balance += out.CoinDetails.GetValue()
		}
		fmt.Println(balance)
		time.Sleep(5 * time.Second)
	}
}
