package main

import (
	"fmt"
	"strconv"

	devframework "github.com/0xkumi/incognito-dev-framework"
)

func main() {
	node := devframework.NewAppNode("fullnode", devframework.TestNet2Param, true, false)
	localnode = node
	CoinProcessedState = make(map[byte]uint64)

	//load CoinProcessedState
	for i := 0; i < 8; i++ {
		statePrefix := fmt.Sprintf("coin-processed-%v", i)
		v, err := localnode.GetUserDatabase().Get([]byte(statePrefix), nil)
		if err != nil {
			fmt.Println(err)
			continue
		}
		height, err := strconv.ParseUint(string(v), 0, 64)
		if err != nil {
			fmt.Println(err)
			continue
		}
		CoinProcessedState[byte(i)] = height
	}
	node.OnNewBlockFromParticularHeight(0, int64(CoinProcessedState[0]), true, OnNewShardBlock)
	select {}
}
