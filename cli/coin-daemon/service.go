package main

import (
	"encoding/json"
	"log"
	"net/http"

	"github.com/incognitochain/incognito-chain/wallet"
)

func startService() {
	http.HandleFunc("/getbalance", getBalanceHandler)
	http.HandleFunc("/importkey", importKeyHandler)
	http.HandleFunc("/getcoinstodecrypt", getCoinsHandler)
	http.HandleFunc("/getdaemonstate", getStateHandler)
	http.HandleFunc("/createtx", createTxHandler)
	http.HandleFunc("/cancelAllTxs", cancelAllTxsHandler)
	err := http.ListenAndServe("127.0.0.1:9000", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}

func getStateHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	return
}

func importKeyHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	if r.Method != "GET" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}
	return
}
func getCoinsHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	if r.Method != "GET" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	key := r.URL.Query().Get("key")
	wl, err := wallet.Base58CheckDeserialize(key)
	if err != nil {
		http.Error(w, "Unexpected error", http.StatusInternalServerError)
		return
	}
	outcoins, err := GetCoins(&wl.KeySet, nil)
	if err != nil {
		http.Error(w, "Unexpected error", http.StatusInternalServerError)
		return
	}
	coinsBytes, err := json.Marshal(outcoins)
	if err != nil {
		http.Error(w, "Unexpected error", http.StatusInternalServerError)
		return
	}
	w.WriteHeader(200)
	_, err = w.Write(coinsBytes)
	if err != nil {
		panic(err)
	}
	return
}

func getBalanceHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	if r.Method != "GET" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	w.WriteHeader(200)
	// _, err = w.Write(sysBytes)
	// if err != nil {
	// 	panic(err)
	// }
	return
}

func cancelAllTxsHandler(w http.ResponseWriter, r *http.Request) {

	return
}

func createTxHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	if r.Method != "GET" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}
}
