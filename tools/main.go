package main

import (
	"fmt"

	"github.com/incognitochain/incognito-chain/wallet"
)

func main() {

	// wl, err := wallet.Base58CheckDeserialize("12RyzNpddn8Pc1eHyJXbcDA25RFMAJFNEMLzRfMt5puEMGh3kziMBuRg3inckbAcEMdwP7KvPsqagwz3KsQhU8cdGH4QtgBkXs3dYMi")
	// if err != nil {
	// 	panic(err)
	// }

	// k := wl.Base58CheckSerialize(1)
	// fmt.Println(k)

	wl2, err := wallet.Base58CheckDeserialize("113KNzNTreF3oWtJGtpyHMHKshDwumKuK5rHrj1cEDSdE7Tz7XaozF5W1P5SB8hMWoEr9w5oR8mv1uwHERwb3nadquqBCZjjngpxZARxM7Tr")
	if err != nil {
		panic(err)
	}

	k2 := wl2.Base58CheckSerialize(0)
	fmt.Println(k2)

	return
}
