package main

import (
	"errors"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"

	"github.com/karalabe/hid"
	"lukechampine.com/flagg"
)

func OpenNanoS() (*NanoS, error) {
	const (
		ledgerVendorID       = 0x2c97
		ledgerNanoSProductID = 0x0001
		//ledgerUsageID        = 0xffa0
	)

	// search for Nano S
	devices := hid.Enumerate(ledgerVendorID, ledgerNanoSProductID)
	if len(devices) == 0 {
		return nil, errors.New("Nano S not detected")
	} // else if len(devices) > 1 {
	// 	return nil, errors.New("Unexpected error -- Is the Incognito wallet app running?")
	// }

	// open the device
	device, err := devices[0].Open()
	if err != nil {
		return nil, err
	}

	// wrap raw device I/O in HID+APDU protocols
	return &NanoS{
		device: &apduFramer{
			hf: &hidFramer{
				rw: device,
			},
		},
	}, nil
}

func parseIndex(s string) uint32 {
	index, err := strconv.ParseUint(s, 10, 32)
	if err != nil {
		log.Fatalln("Couldn't parse index:", err)
	} else if index > math.MaxUint32 {
		log.Fatalf("Index too large (max %v)", math.MaxUint32)
	}
	return uint32(index)
}

const (
	rootUsage = `Usage:
    incognitoledger [flags] [action]

Actions:
    addr            generate an address
    pubkey          generate a pubkey
    hash            sign a trusted hash
    txn             sign a transaction
`
	debugUsage = `print raw APDU exchanges`

	versionUsage = `Usage:
	sialedger version

Prints the version of the sialedger binary, as well as the version reported by
the Sia Ledger Nano S app (if available).
`
	addrUsage = `Usage:
	sialedger addr [key index]

Generates an address using the public key with the specified index.
`
	privUsage          = ``
	viewKeyUsage       = ``
	importPrivUsage    = ``
	genCommitmentUsage = ``
	genOTAUsage        = ``
	genRingSigUsage    = ``
	genProofUsage      = ``
	genAssetTagUsage   = ``
	genKeyImageUsage   = ``
	encryptCoinUsage   = ``
	decryptCoinUsage   = ``
)

func main() {
	log.SetFlags(0)
	rootCmd := flagg.Root
	rootCmd.Usage = flagg.SimpleUsage(rootCmd, rootUsage)
	rootCmd.BoolVar(&DEBUG, "apdu", false, debugUsage)

	versionCmd := flagg.New("version", versionUsage)
	addrCmd := flagg.New("addr", addrUsage)
	privCmd := flagg.New("priv", privUsage)
	getViewKeyCmd := flagg.New("view", viewKeyUsage)
	importPrivateKeyCmd := flagg.New("importpriv", importPrivUsage)
	genCommitmentCmd := flagg.New("gencommitment", genCommitmentUsage)
	genOTACmd := flagg.New("genota", genOTAUsage)
	genRingSigCmd := flagg.New("genringsig", genRingSigUsage)
	genProofCmd := flagg.New("genproof", genProofUsage)
	genAssetTagCmd := flagg.New("genassettag", genAssetTagUsage)
	genKeyImageCmd := flagg.New("genkeyimage", genKeyImageUsage)
	encryptCoinCmd := flagg.New("encoin", encryptCoinUsage)
	decryptCoinCmd := flagg.New("decoin", decryptCoinUsage)

	cmd := flagg.Parse(flagg.Tree{
		Cmd: rootCmd,
		Sub: []flagg.Tree{
			{Cmd: versionCmd},
			{Cmd: addrCmd},
			{Cmd: privCmd},
			{Cmd: getViewKeyCmd},
			{Cmd: importPrivateKeyCmd},
			{Cmd: genCommitmentCmd},
			{Cmd: genOTACmd},
			{Cmd: genRingSigCmd},
			{Cmd: genProofCmd},
			{Cmd: genAssetTagCmd},
			{Cmd: genKeyImageCmd},
			{Cmd: encryptCoinCmd},
			{Cmd: decryptCoinCmd},
		},
	})
	args := cmd.Args()

	var nanos *NanoS
	if cmd != rootCmd && cmd != versionCmd {
		var err error
		nanos, err = OpenNanoS()
		if err != nil {
			log.Fatalln("Couldn't open device:", err)
		}
	}

	switch cmd {
	case rootCmd:
		if len(args) != 0 {
			rootCmd.Usage()
			return
		}
		fallthrough
	case versionCmd:
		// try to get Nano S app version
		var appVersion string
		nanos, err := OpenNanoS()
		if err != nil {
			appVersion = "(could not connect to Nano S)"
		} else if appVersion, err = nanos.GetVersion(); err != nil {
			appVersion = "(could not read version from Nano S: " + err.Error() + ")"
		}

		fmt.Printf("%s v0.1.0\n", os.Args[0])
		fmt.Println("Nano S app version:", appVersion)

	case addrCmd:
		if len(args) != 1 {
			addrCmd.Usage()
			return
		}
		addr, err := nanos.GetAddress(parseIndex(args[0]))
		if err != nil {
			log.Fatalln("Couldn't get address:", err)
		}
		fmt.Println(addr)
	case privCmd:
		if len(args) != 1 {
			privCmd.Usage()
			return
		}
		priv, err := nanos.GetPrivateKey(parseIndex(args[0]))
		if err != nil {
			log.Fatalln("Couldn't get address:", err)
		}
		fmt.Println(priv)
	case getViewKeyCmd:
		err := nanos.GetViewKey()
		if err != nil {
			log.Fatalln(err)
		}
	case importPrivateKeyCmd:
		err := nanos.ImportPrivateKey()
		if err != nil {
			log.Fatalln(err)
		}
	case genCommitmentCmd:
		err := nanos.GenCommitment()
		if err != nil {
			log.Fatalln(err)
		}
	case genOTACmd:
		err := nanos.GenOTA()
		if err != nil {
			log.Fatalln(err)
		}
	case genRingSigCmd:
		err := nanos.GenRingSig()
		if err != nil {
			log.Fatalln(err)
		}
	case genProofCmd:
		err := nanos.GenProof()
		if err != nil {
			log.Fatalln(err)
		}
	case genAssetTagCmd:
		err := nanos.GenAssetTag()
		if err != nil {
			log.Fatalln(err)
		}
	case genKeyImageCmd:
		err := nanos.GenKeyImage()
		if err != nil {
			log.Fatalln(err)
		}
	case encryptCoinCmd:
		err := nanos.EncryptCoin()
		if err != nil {
			log.Fatalln(err)
		}
	case decryptCoinCmd:
		err := nanos.DecryptCoin()
		if err != nil {
			log.Fatalln(err)
		}
		// case pubkeyCmd:
		// 	if len(args) != 1 {
		// 		pubkeyCmd.Usage()
		// 		return
		// 	}
		// 	pubkey, err := nanos.GetPublicKey(parseIndex(args[0]))
		// 	if err != nil {
		// 		log.Fatalln("Couldn't get public key:", err)
		// 	}
		// 	pk := types.Ed25519PublicKey(pubkey)
		// 	fmt.Println(pk.String())

		// case hashCmd:
		// 	if len(args) != 2 {
		// 		hashCmd.Usage()
		// 		return
		// 	}
		// 	var hash [32]byte
		// 	hashBytes, err := hex.DecodeString(args[0])
		// 	if err != nil {
		// 		log.Fatalln("Couldn't read hash:", err)
		// 	} else if len(hashBytes) != 32 {
		// 		log.Fatalf("Wrong hex hash length (%v, wanted 32)", len(hashBytes))
		// 	}
		// 	copy(hash[:], hashBytes)

		// 	sig, err := nanos.SignHash(hash, parseIndex(args[1]))
		// 	if err != nil {
		// 		log.Fatalln("Couldn't get signature:", err)
		// 	}
		// 	fmt.Println(base64.StdEncoding.EncodeToString(sig[:]))

		// case txnCmd:
		// 	if (*txnHash && len(args) != 2) || (!*txnHash && len(args) != 3) {
		// 		txnCmd.Usage()
		// 		return
		// 	}
		// 	txnBytes, err := ioutil.ReadFile(args[0])
		// 	if err != nil {
		// 		log.Fatalln("Couldn't read transaction:", err)
		// 	}
		// 	var txn types.Transaction
		// 	if err := json.Unmarshal(txnBytes, &txn); err != nil {
		// 		log.Fatalln("Couldn't decode transaction:", err)
		// 	}
		// 	sigIndex := uint16(parseIndex(args[1]))

		// 	if *txnHash {
		// 		sighash, err := nanos.CalcTxnHash(txn, sigIndex)
		// 		if err != nil {
		// 			log.Fatalln("Couldn't get hash:", err)
		// 		}
		// 		fmt.Println(hex.EncodeToString(sighash[:]))
		// 	} else {
		// 		sig, err := nanos.SignTxn(txn, sigIndex, parseIndex(args[2]))
		// 		if err != nil {
		// 			log.Fatalln("Couldn't get signature:", err)
		// 		}
		// 		fmt.Println(base64.StdEncoding.EncodeToString(sig[:]))
		// 	}
	}
}
