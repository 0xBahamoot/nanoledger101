package main

import (
	"sync"
	"time"

	"github.com/incognitochain/incognito-chain/privacy/key"
)

type Account struct {
	PAstr          string //PaymentAddressString
	PaymentAddress key.PaymentAddress
	Viewkey        key.ViewingKey
	OTAKey         key.PrivateOTAKey

	lock sync.RWMutex
	//map[tokenID][]coinHash
	PendingCoins   map[string][]string //wait for tx to confirm
	AvaliableCoins map[string][]string //avaliable to use
	EncryptedCoins map[string][]string //encrypted, dont know whether been used
}

type AccountState struct {
	Account *Account
	Balance uint64
}

var accountListLck sync.RWMutex
var accountList map[string]*AccountState
var currentAccount string

func importAccount(name string, paymentAddr string, viewOTAKey string) error {
	return nil
}

func initAccountService() error {
	accountList = make(map[string]*AccountState)
	return nil
}

func scanForNewCoins() {
	for {
		if len(accountList) == 0 {
			time.Sleep(15 * time.Second)
			continue
		}
		accountListLck.RLock()
		for name, account := range accountList {
			_ = account
			_ = name
		}
		accountListLck.RUnlock()
		time.Sleep(40 * time.Second)
	}
}

func getAccountList() map[string]string {
	var result map[string]string
	result = make(map[string]string)
	accountListLck.RLock()
	for name, account := range accountList {
		result[name] = account.Account.PAstr
	}
	accountListLck.RUnlock()
	return result
}

func getAllBalance() map[string]uint64 {
	var result map[string]uint64

	accountListLck.RLock()
	result = make(map[string]uint64)
	for name, account := range accountList {
		result[name] = account.Balance
	}
	accountListLck.RUnlock()
	return result
}

func updateAvaliableCoin(account *Account, keyimages []string) error {
	return nil
}

func chooseCoinsForAccount(account *Account, amount int64, tokenID string) ([]string, error) {
	return nil, nil
}
