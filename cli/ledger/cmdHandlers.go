package main

import (
	"bytes"
	"encoding/binary"
	"encoding/hex"
	"errors"
	"fmt"

	"gitlab.com/NebulousLabs/Sia/types"
)

const (
	cmdGetVersion       = 0x01
	cmdGetAddress       = 0x02
	cmdGetViewKey       = 0x03
	cmdGetPrivateKey    = 0x04
	cmdImportPrivateKey = 0x05
	cmdGenCommitment    = 0x06
	cmdGenOTA           = 0x07
	cmdGenRingSig       = 0x08
	cmdGenProof         = 0x09
	cmdGenAssetTag      = 0x10
	cmdKeyImage         = 0x11
	cmdEncryptCoin      = 0x50
	cmdDecryptCoin      = 0x51
	cmdImportSeed       = 0x90

	p1First = 0x00
	p1More  = 0x80

	p2DisplayAddress = 0x00
	p2DisplayPubkey  = 0x01
	p2DisplayHash    = 0x00
	p2SignHash       = 0x01
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

// func (n *NanoS) GetPublicKey(index uint32) (pubkey [32]byte, err error) {
// 	encIndex := make([]byte, 4)
// 	binary.LittleEndian.PutUint32(encIndex, index)

// 	resp, err := n.Exchange(cmdGetAddress, 0, p2DisplayPubkey, encIndex)
// 	if err != nil {
// 		return [32]byte{}, err
// 	}
// 	if copy(pubkey[:], resp) != len(pubkey) {
// 		return [32]byte{}, errors.New("pubkey has wrong length")
// 	}
// 	return
// }

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

func (n *NanoS) ImportPrivateKey() error {
	buf := new(bytes.Buffer)
	bs, _ := hex.DecodeString("00000000000214666ccc56b88d4d8d3f5fae61f1f06d9620327fe259157272016dfe54ef6fef203b50d18fae649b7dfa6e3e3078f9ba83142f993acd55d6649abc28b7979d7a08")
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
	resp, err := n.Exchange(cmdKeyImage, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
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