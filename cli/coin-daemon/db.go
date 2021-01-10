package main

import (
	"path/filepath"

	"github.com/incognitochain/incognito-chain/incdb"
)

var accountDB *incdb.Database
var keyimageDB *incdb.Database

func initAccountDB(datadir string) error {
	temp, err := incdb.Open("leveldb", filepath.Join(datadir, "accounts"))
	if err != nil {
		return err
	}
	accountDB = &temp
	return nil
}

func initKeyimageDB(datadir string) error {
	temp, err := incdb.Open("leveldb", filepath.Join(datadir, "keyimages"))
	if err != nil {
		return err
	}
	keyimageDB = &temp
	return nil
}

func saveAccount(account Account) error {
	return nil
}

func loadAccountsFromDB() ([]*Account, error) {
	var result []*Account

	return result, nil
}

func saveKeyImageList(keyImage [][]byte, paymentAddrHash string, commitmentHash string) error {

	return nil
}
