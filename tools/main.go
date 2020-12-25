package main

import (
	"fmt"

	"github.com/incognitochain/incognito-chain/wallet"
)

func main() {

	wl, err := wallet.Base58CheckDeserialize("12RyzNpddn8Pc1eHyJXbcDA25RFMAJFNEMLzRfMt5puEMGh3kziMBuRg3inckbAcEMdwP7KvPsqagwz3KsQhU8cdGH4QtgBkXs3dYMi")
	if err != nil {
		panic(err)
	}

	k := wl.Base58CheckSerialize(1)
	fmt.Println(k)

	wl2, err := wallet.Base58CheckDeserialize("111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnPDGEQMGvA1pRfT71Crr7MM2ShvpkxCBWBL2icG22cXSpB8A2XKuezTJ")
	if err != nil {
		panic(err)
	}

	k2 := wl2.Base58CheckSerialize(1)
	fmt.Println(k2)

	return
}
