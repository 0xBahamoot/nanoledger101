package main

import (
	"bytes"
	"encoding/hex"
	"fmt"

	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/privacy/coin"
	"github.com/incognitochain/incognito-chain/privacy/key"
	"github.com/incognitochain/incognito-chain/privacy/operation"
	"github.com/incognitochain/incognito-chain/wallet"
)

func main() {
	var coinListStr map[string][]string
	coinListStr = make(map[string][]string)

	coinListStr["111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnPDGEQMGvA1pRfT71Crr7MM2ShvpkxCBWBL2icG22cXSpB8A2XKuezTJ"] = []string{"02002017fd6aff8fecd18243af1a83dab0e47ca5fafec256ba497b3136a6b3f68eecb1204c9ec01d8daa3ed381176e3d818f96be9ed202e7d08c6ba413fd0d75e7bcae4600000044000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000020e0ebfd56cd436ff8b56fd8e55d3979d52832e469effc568639c3e85a744a5e0420102700000000000000000000000000000000000000000000000000000000000000"}

	nanos, err := OpenNanoS()
	if err != nil {
		panic(err)
	}
	err = nanos.TrustDevice()
	if err != nil {
		panic(err)
	}
	nanos.device.deviceIO.Close()

	for privateKey, coinList := range coinListStr {
		wl, err := wallet.Base58CheckDeserialize(privateKey)
		if err != nil {
			panic(err)
		}
		keyBytes := make([]byte, 0)
		keyBytes = append(keyBytes, byte(0))
		keyBytes = append(keyBytes, wl.Depth)
		keyBytes = append(keyBytes, wl.ChildNumber...)
		keyBytes = append(keyBytes, wl.ChainCode...)
		keyBytes = append(keyBytes, byte(len(wl.KeySet.PrivateKey)))
		keyBytes = append(keyBytes, wl.KeySet.PrivateKey[:]...)
		fmt.Println(hex.EncodeToString(keyBytes))
		nanos, err := OpenNanoS()
		if err != nil {
			panic(err)
		}
		err = nanos.ImportPrivateKey(keyBytes)
		if err != nil {
			panic(err)
		}
		nanos.device.deviceIO.Close()
		for _, coinHex := range coinList {
			c, coinPubKey, kOTA, err := extractCoinData(coinHex, wl.KeySet.OTAKey)
			if err != nil {
				panic(err)
			}
			nanos, err := OpenNanoS()
			if err != nil {
				panic(err)
			}
			decryptedKeyImageDevice, err := nanos.GenKeyImage(kOTA, coinPubKey)
			if err != nil {
				panic(err)
			}
			nanos.device.deviceIO.Close()
			plc, err := c.Decrypt(&wl.KeySet)
			if err != nil {
				panic(err)
			}
			decryptedKeyImageHost := plc.GetKeyImage().ToBytesS()

			if bytes.Compare(decryptedKeyImageHost, decryptedKeyImageDevice) != 0 {
				fmt.Println("decrypt err for coin", coinHex)
				continue
			}
			fmt.Println("decrypt success coin", coinHex[:16])
		}
	}
}

func extractCoinData(coinHex string, OTAKey key.OTAKey) (c coin.CoinV2, coinPubKey []byte, kOTA []byte, err error) {
	c.Init()
	c2Bytes, _ := hex.DecodeString(coinHex)
	err = c.SetBytes(c2Bytes)
	if err != nil {
		panic(err)
	}
	_, txRandomOTAPoint, index, err := c.GetTxRandomDetail()
	if err != nil {
		panic(err)
	}
	rK := new(operation.Point).ScalarMult(txRandomOTAPoint, OTAKey.GetOTASecretKey())
	H := operation.HashToScalar(append(rK.ToBytesS(), common.Uint32ToBytes(index)...))
	kOTA = H.ToBytesS()
	coinPubKey = c.GetPublicKey().ToBytesS()
	return
}
