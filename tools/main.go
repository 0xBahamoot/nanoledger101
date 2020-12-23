package main

import (
	"fmt"

	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/common/base58"
	"github.com/incognitochain/incognito-chain/wallet"
)

func main() {
	// DgXDCEGF8jZpMukUvGydD22GvxePVxa6D8VT4u7JiiTBtxXPKmmBz6MWMotUyDPhtPQahfRDBftpmhytzM9KjJnB6cadhKea
	// 111116JSC6cHQQ5rMCG1WXp6YsgRC9SaUeL1XJeVczbAK4pU1vPU6tGXYW6edJcjqQsX7dGsBN1AAvsUWn6og8SAyCUQUqywCjFs

	// 12RyzNpddn8Pc1eHyJXbcDA25RFMAJFNEMLzRfMt5puEMGh3rtFdMsSHupaKfXn9KQmPZiRLbygzktCW1H5zGDwYHHjnMai41QVm66m
	// b := base58.Base58{}.Decode("12RyzNpddn8Pc1eHyJXbcDA25RFMAJFNEMLzRfMt5puEMGh3kziMBuRg3inckbAcEMdwP7KvPsqagwz3KsQhU8cdGH4QtgBkXrFarx2")
	b, v, err := base58.Base58Check{}.Decode("12RyzNpddn8Pc1eHyJXbcDA25RFMAJFNEMLzRfMt5puEMGh3kziMBuRg3inckbAcEMdwP7KvPsqagwz3KsQhU8cdGH4QtgBkXrFarx2")
	fmt.Println(v, err)
	s := base58.Base58Check{}.Encode(b, common.ZeroByte)
	fmt.Println(s)
	// encoded := s
	// decoded, version, err := btcbase58.CheckDecode(encoded)
	// if err != nil {
	// 	fmt.Println(err)
	// 	return
	// }
	// // Show the decoded data.
	// fmt.Printf("Decoded data: %x\n", decoded)
	// fmt.Println("Version Byte:", version)

	wl, err := wallet.Base58CheckDeserialize(s)
	if err != nil {
		panic(err)
	}
	// bw, _ := json.Marshal(wl)
	// fmt.Println(string(bw))
	// tt := binary.BigEndian.Uint32(wl.ChildNumber)
	// fmt.Println(tt)

	k := wl.Base58CheckSerialize(1)
	fmt.Println(k)
	return
}
