package main

import (
	"encoding/hex"
	"encoding/json"
	"flag"
	"fmt"
	"time"

	devframework "github.com/0xkumi/incognito-dev-framework"
	"github.com/0xkumi/incognito-dev-framework/account"
	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/privacy/coin"
	"github.com/incognitochain/incognito-chain/privacy/operation"
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
	/////////////////////
	wl2, _ := wallet.Base58CheckDeserialize("111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnPDGEQMGvA1pRfT71Crr7MM2ShvpkxCBWBL2icG22cXSpB8A2XKuezTJ")
	wl3, _ := wallet.Base58CheckDeserialize("111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnLuW4S6ReEGiUMbQKQNq84Fw465AuwVGhDiAkHNBqvmJ81PUsspZn37R")

	wl2Bytes, _ := json.Marshal(wl2)

	wl3Bytes, _ := json.Marshal(wl3)

	fmt.Printf("wl2 %s\n", wl2Bytes)

	fmt.Printf("wl3 %s\n", wl3Bytes)

	fmt.Println()
	fmt.Println("wl2Addr", wl2.Base58CheckSerialize(1))
	fmt.Println("wl3Addr", wl3.Base58CheckSerialize(1))
	// e0e29d899c50c2df14e38be0363a93ade833a2446d687449c8baf6ad88ae6ce6
	var c2 coin.CoinV2
	c2.Init()
	c2Bytes, _ := hex.DecodeString("02002017fd6aff8fecd18243af1a83dab0e47ca5fafec256ba497b3136a6b3f68eecb1204c9ec01d8daa3ed381176e3d818f96be9ed202e7d08c6ba413fd0d75e7bcae4600000044000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000020e0ebfd56cd436ff8b56fd8e55d3979d52832e469effc568639c3e85a744a5e0420102700000000000000000000000000000000000000000000000000000000000000")
	err := c2.SetBytes(c2Bytes)
	if err != nil {
		panic(err)
	}

	OTAKey := wl2.KeySet.OTAKey

	_, txRandomOTAPoint, index, err := c2.GetTxRandomDetail()
	if err != nil {
		panic(err)
	}
	rK := new(operation.Point).ScalarMult(txRandomOTAPoint, OTAKey.GetOTASecretKey())  //(r_ota*G) * k = r_ota * K
	H := operation.HashToScalar(append(rK.ToBytesS(), common.Uint32ToBytes(index)...)) // Hash(r_ota*K, index)

	Hstr := hex.EncodeToString(H.ToBytesS())
	fmt.Println("Hstr", Hstr, len(H.ToBytesS()))
	CPubk := hex.EncodeToString(c2.GetPublicKey().ToBytesS())
	CPubKey := c2.GetPublicKey().GetKey()
	pubKeyHash := CPubKey.HashToPoint().ToBytes()

	pubkeyHash2 := operation.HashToPoint(c2.GetPublicKey().ToBytesS())

	fmt.Println("pubKeyHash2(I)", hex.EncodeToString(pubkeyHash2.ToBytesS()))

	fmt.Println("pubKeyHash(I)", hex.EncodeToString(pubKeyHash[:]))
	// ed25519.
	fmt.Println("CPubk", CPubk)

	k := new(operation.Scalar).FromBytesS(wl2.KeySet.PrivateKey) //already done on ledger
	k1 := new(operation.Scalar).Add(H, k)
	k1Str := hex.EncodeToString(k1.ToBytesS())
	fmt.Println("k1", k1Str, len(k1.ToBytesS()))

	kimg := new(operation.Point).ScalarMult(pubkeyHash2, k1)
	fmt.Println(hex.EncodeToString(kimg.ToBytesS()))

	cpl, err := c2.Decrypt(&wl2.KeySet)
	if err != nil {
		panic(err)
	}

	fmt.Println(hex.EncodeToString(cpl.GetKeyImage().ToBytesS()))
	panic(0)
	///////////////////
	switch *modeFlag {
	case MODELIGHT:
		node := devframework.NewAppNode("fullnode", devframework.TestNet2Param, true, false)
		localnode = node
		initCoinService()
	case MODERPC:
		node := devframework.NewRPCClient("")
		_ = node
	case MODESIM:
		node := devframework.NewStandaloneSimulation("simnode", devframework.Config{
			ChainParam: devframework.NewChainParam(devframework.ID_TESTNET2).SetActiveShardNumber(8),
			DisableLog: true,
		})
		node.GenerateBlock().NextRound()
		node.ShowBalance(node.GenesisAccount)
		// acc1 := node.NewAccountFromShard(0)
		// acc2 := node.NewAccountFromShard(0)
		acc0, _ := account.NewAccountFromPrivatekey("111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnPDGEQMGvA1pRfT71Crr7MM2ShvpkxCBWBL2icG22cXSpB8A2XKuezTJ")

		// fmt.Println(acc1.PrivateKey)
		// fmt.Println(acc2.PrivateKey)
		// node.ShowBalance(acc1)
		// node.ShowBalance(acc2)
		node.Pause()
		// node.RPC.API_SendTxPRV()
		// node.SendPRV(node.GenesisAccount, acc1, 1000, acc2, 1000)
		node.SendPRV(node.GenesisAccount, acc0, 10000)
		for i := 0; i < 10; i++ {
			node.GenerateBlock().NextRound()
		}

		l, err := node.RPC.API_ListOutputCoins(acc0.PrivateKey)
		if err != nil {
			panic(err)
		}
		// fmt.Println(l.Outputs)
		// node.ShowBalance(acc1)
		// node.ShowBalance(acc2)
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

				cpl, err := cv2.Decrypt(acc0.Keyset)
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

	wl, _ := wallet.Base58CheckDeserialize("111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnPDGEQMGvA1pRfT71Crr7MM2ShvpkxCBWBL2icG22cXSpB8A2XKuezTJ")
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
