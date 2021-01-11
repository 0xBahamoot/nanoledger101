package main

import (
	"bytes"
	"encoding/binary"
	"encoding/hex"
	"errors"
	"fmt"

	"gitlab.com/NebulousLabs/Sia/types"
)

func (n *NanoS) GetVersion() (version string, err error) {
	resp, err := n.Exchange(cmdGetVersion, 0, 0, nil)
	if err != nil {
		return "", err
	} else if len(resp) != 3 {
		fmt.Printf("%v\n", resp)
		return "", errors.New("version has wrong length")
	}
	return fmt.Sprintf("v%d.%d.%d", resp[0], resp[1], resp[2]), nil
}

func (n *NanoS) GetAddress(index uint32) (addr types.UnlockHash, err error) {
	encIndex := make([]byte, 4)
	binary.LittleEndian.PutUint32(encIndex, index)

	resp, err := n.Exchange(cmdGetAddress, 0, 0, nil)
	if err != nil {
		return types.UnlockHash{}, err
	}
	fmt.Printf("address %v\n", resp[:])
	fmt.Println("\n", string(resp[:]))
	// fmt.Println("address:", string(resp))
	return
}

func (n *NanoS) GetPrivateKey(index uint32) (addr types.UnlockHash, err error) {
	encIndex := make([]byte, 4)
	binary.LittleEndian.PutUint32(encIndex, index)

	resp, err := n.Exchange(cmdGetPrivateKey, 0, p2DisplayAddress, encIndex)
	if err != nil {
		return types.UnlockHash{}, err
	}
	fmt.Println("privatekey:", string(resp))
	// err = addr.LoadString(string(resp[32:]))
	return
}

func (n *NanoS) GetViewKey() error {
	resp, err := n.Exchange(cmdGetViewKey, 0, 0, nil)
	if err != nil {
		return err
	}
	fmt.Printf("viewkey: %v", resp)
	// fmt.Println("viewkey:", string(resp[:]))
	return nil
}

func (n *NanoS) GetOTAKey() error {
	resp, err := n.Exchange(cmdGetOTAKey, 0, 0, nil)
	if err != nil {
		return err
	}
	fmt.Printf("ota: %v", resp)
	// fmt.Println("viewkey:", string(resp[:]))
	return nil
}

func (n *NanoS) ImportPrivateKey() error {
	buf := new(bytes.Buffer)
	// 000100000020812566598706f6f772fa0ec67e5efaac12c85a64b730518077a432fd3cb97a8c20063632b2a159e45002394460aee02de54d2b8926d236f45be2e077dcc81d0d04
	bs, _ := hex.DecodeString("00013b36d59d480b6a8a78e73c9614c8f9b23099e5a9f2ebc796b76ed593aa9629a96107e71f20805097644ec46fc8a2ae9114d2ca9eef9c677ccd6c8ce97aed623003e62a1102")
	buf.Write(bs)
	// buf.WriteString("111111bgk2j6vZQvzq8tkonDLLXEvLkMwBMn5BoLXLpf631boJnJ1UgJnLBzXe4qSMXGJAKw1LdKmfWZDNkhd24gkb2oqbs4q9UgjJZDvq")

	resp, err := n.Exchange(cmdImportPrivateKey, 0, 0, buf.Next(255))
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) GenCommitment() error {
	resp, err := n.Exchange(cmdGenCommitment, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) GenOTA() error {
	resp, err := n.Exchange(cmdGenOTA, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) GenRingSig() error {
	resp, err := n.Exchange(cmdGenRingSig, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) GenProof() error {
	resp, err := n.Exchange(cmdGenProof, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) GenAssetTag() error {
	resp, err := n.Exchange(cmdGenAssetTag, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) GenKeyImage() error {
	buf := new(bytes.Buffer)
	bs, _ := hex.DecodeString("c4541151e39bb43e7b00ad6a1d999d609f5939ca622a9db7b7391c5190eea909")
	buf.Write(bs)
	bs1, _ := hex.DecodeString("17fd6aff8fecd18243af1a83dab0e47ca5fafec256ba497b3136a6b3f68eecb1")
	buf.Write(bs1)

	resp, err := n.Exchange(cmdKeyImage, 0, 0, buf.Next(255))
	if err != nil {
		return err
	}
	// fmt.Println("coin_pubkey", hex.EncodeToString(resp[:32]))
	// fmt.Println("I", hex.EncodeToString(resp[32:len(resp)-1]))
	fmt.Println("I", hex.EncodeToString(resp))
	return nil
}

func (n *NanoS) EncryptCoin() error {
	resp, err := n.Exchange(cmdEncryptCoin, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) DecryptCoin() error {
	resp, err := n.Exchange(cmdDecryptCoin, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) GetValidatorKey() error {
	resp, err := n.Exchange(cmdGetValidatorKey, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

// func (n *NanoS) SignHash(hash [32]byte, keyIndex uint32) (sig [64]byte, err error) {
// 	encIndex := make([]byte, 4)
// 	binary.LittleEndian.PutUint32(encIndex, keyIndex)

// 	resp, err := n.Exchange(cmdSignHash, 0, 0, append(encIndex, hash[:]...))
// 	if err != nil {
// 		return [64]byte{}, err
// 	}
// 	if copy(sig[:], resp) != len(sig) {
// 		return [64]byte{}, errors.New("signature has wrong length")
// 	}
// 	return
// }

// func (n *NanoS) CalcTxnHash(txn types.Transaction, sigIndex uint16) (hash [32]byte, err error) {
// 	buf := new(bytes.Buffer)
// 	binary.Write(buf, binary.LittleEndian, uint32(0)) // keyIndex
// 	binary.Write(buf, binary.LittleEndian, sigIndex)
// 	txn.MarshalSia(buf)

// 	var resp []byte
// 	for buf.Len() > 0 {
// 		var p1 byte = p1More
// 		if resp == nil {
// 			p1 = p1First
// 		}
// 		resp, err = n.Exchange(cmdCalcTxnHash, p1, p2DisplayHash, buf.Next(255))
// 		if err != nil {
// 			return [32]byte{}, err
// 		}
// 	}
// 	if copy(hash[:], resp) != len(hash) {
// 		return [32]byte{}, errors.New("hash has wrong length")
// 	}
// 	return
// }

// func (n *NanoS) SignTxn(txn types.Transaction, sigIndex uint16, keyIndex uint32) (sig [64]byte, err error) {
// 	buf := new(bytes.Buffer)
// 	binary.Write(buf, binary.LittleEndian, keyIndex)
// 	binary.Write(buf, binary.LittleEndian, sigIndex)
// 	txn.MarshalSia(buf)

// 	var resp []byte
// 	for buf.Len() > 0 {
// 		var p1 byte = p1More
// 		if resp == nil {
// 			p1 = p1First
// 		}
// 		resp, err = n.Exchange(cmdCalcTxnHash, p1, p2SignHash, buf.Next(255))
// 		if err != nil {
// 			return [64]byte{}, err
// 		}
// 	}
// 	if copy(sig[:], resp) != len(sig) {
// 		return [64]byte{}, errors.New("signature has wrong length")
// 	}
// 	return
// }
