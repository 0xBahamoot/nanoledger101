package main

import (
	"encoding/binary"
	"encoding/json"
	"fmt"

	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/common/base58"
	"github.com/incognitochain/incognito-chain/wallet"
)

func main() {
	b := base58.Base58{}.Decode("111116JSC6cHQQ5rMCG1WXp6YsgRC9SaUeL1XJeVczbAK4pU1vQGDn2qDp8KyF4NES9hqJMsLeqGT3YMWSrbn9Lkw5Kpz4wyJW9T")
	s := base58.Base58Check{}.Encode(b, common.ZeroByte)
	fmt.Println(s)
	wl, err := wallet.Base58CheckDeserialize(s)
	if err != nil {
		panic(err)
	}
	bw, _ := json.Marshal(wl)
	fmt.Println(string(bw))
	tt := binary.BigEndian.Uint32(wl.ChildNumber)
	fmt.Println(tt)

	k := wl.Base58CheckSerialize(0)
	fmt.Println(k)
	return
}

// fmt.Printf("%v %v\n", cs1, cs2)
// dst := make([]byte, hex.EncodedLen(len(data)-4))
// hex.Encode(dst, data[0:len(data)-4])

// // fmt.Printf("%s\n", dst)

// src := []byte("0a205559dc747d92031ce2f4f08cd1be9d7d4d94566005c28f02c85ef4683450")
// dst2 := make([]byte, hex.DecodedLen(len(src)))
// _, err := hex.Decode(dst2, src)
// if err != nil {
// 	log.Fatal(err)
// }

// fmt.Printf("%v\n", dst2[:4])
