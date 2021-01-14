package main

import (
	"fmt"
	"path/filepath"

	"github.com/incognitochain/incognito-chain/incdb"
)

var accountDB incdb.Database
var keyimageDB incdb.Database

func initAccountDB(datadir string) error {
	temp, err := incdb.Open("leveldb", filepath.Join(datadir, "accounts"))
	if err != nil {
		return err
	}
	accountDB = temp
	return nil
}

func initKeyimageDB(datadir string) error {
	temp, err := incdb.Open("leveldb", filepath.Join(datadir, "keyimages"))
	if err != nil {
		return err
	}
	keyimageDB = temp
	return nil
}

func saveAccount(account Account) error {
	return nil
}

func loadAccountsFromDB() ([]*Account, error) {
	var result []*Account
	accountListLck.RLock()
	for _, account := range accountList {
		_ = account
	}
	accountListLck.RUnlock()
	return result, nil
}

func saveKeyImageList(keyImages map[string][]byte, tokenID string, paymentAddrHash string) error {
	batch := keyimageDB.NewBatch()

	for commitmentHash, keyImage := range keyImages {
		key := fmt.Sprintf("km-%s-%s-%s", tokenID, paymentAddrHash, commitmentHash)
		err := batch.Put([]byte(key), keyImage)
		if err != nil {
			return err
		}
	}
	if err := batch.Write(); err != nil {
		return err
	}

	return nil
}
