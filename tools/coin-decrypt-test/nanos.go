package main

import (
	"bytes"
	"encoding/binary"
	"encoding/hex"
	"errors"
	"fmt"
	"io"

	"github.com/zondax/hid"
)

var DEBUG bool

type hidFramer struct {
	rw  io.ReadWriter
	seq uint16
	buf [64]byte
	pos int
}

func (hf *hidFramer) Reset() {
	hf.seq = 0
}

func (hf *hidFramer) Write(p []byte) (int, error) {
	if DEBUG {
		fmt.Println("HID <=", hex.EncodeToString(p))
	}
	// split into 64-byte chunks
	chunk := make([]byte, 64)
	binary.BigEndian.PutUint16(chunk[:2], 0x0101)
	chunk[2] = 0x05
	var seq uint16
	buf := new(bytes.Buffer)
	binary.Write(buf, binary.BigEndian, uint16(len(p)))
	buf.Write(p)
	for buf.Len() > 0 {
		binary.BigEndian.PutUint16(chunk[3:5], seq)
		n, _ := buf.Read(chunk[5:])
		if n, err := hf.rw.Write(chunk[:5+n]); err != nil {
			return n, err
		}
		seq++
	}
	return len(p), nil
}

func (hf *hidFramer) Read(p []byte) (int, error) {
	if hf.seq > 0 && hf.pos != 64 {
		// drain buf
		n := copy(p, hf.buf[hf.pos:])
		hf.pos += n
		return n, nil
	}
	// read next 64-byte packet
	if n, err := hf.rw.Read(hf.buf[:]); err != nil {
		return 0, err
	} else if n != 64 {
		panic("read less than 64 bytes from HID")
	}
	// parse header
	channelID := binary.BigEndian.Uint16(hf.buf[:2])
	commandTag := hf.buf[2]
	seq := binary.BigEndian.Uint16(hf.buf[3:5])
	if channelID != 0x0101 {
		return 0, fmt.Errorf("bad channel ID 0x%x", channelID)
	} else if commandTag != 0x05 {
		return 0, fmt.Errorf("bad command tag 0x%x", commandTag)
	} else if seq != hf.seq {
		return 0, fmt.Errorf("bad sequence number %v (expected %v)", seq, hf.seq)
	}
	hf.seq++
	// start filling p
	n := copy(p, hf.buf[5:])
	hf.pos = 5 + n
	return n, nil
}

type APDU struct {
	CLA     byte
	INS     byte
	P1, P2  byte
	Payload []byte
}

type apduFramer struct {
	hf       *hidFramer
	buf      [2]byte // to read APDU length prefix
	deviceIO *hid.Device
}

func (af *apduFramer) Exchange(apdu APDU) ([]byte, error) {
	if len(apdu.Payload) > 255 {
		panic("APDU payload cannot exceed 255 bytes")
	}
	af.hf.Reset()
	data := append([]byte{
		apdu.CLA,
		apdu.INS,
		apdu.P1, apdu.P2,
		byte(len(apdu.Payload)),
	}, apdu.Payload...)
	if _, err := af.hf.Write(data); err != nil {
		return nil, err
	}

	// read APDU length
	if _, err := io.ReadFull(af.hf, af.buf[:]); err != nil {
		return nil, err
	}
	// read APDU payload
	respLen := binary.BigEndian.Uint16(af.buf[:2])
	resp := make([]byte, respLen)
	n, _ := io.ReadFull(af.hf, resp)
	if DEBUG {
		fmt.Println("HID =>", respLen, n, hex.EncodeToString(resp))
	}

	return resp, nil
}

type NanoS struct {
	device *apduFramer
}

type ErrCode uint16

func (c ErrCode) Error() string {
	return fmt.Sprintf("Error code 0x%x", uint16(c))
}

const codeSuccess = 0x9000
const codeUserRejected = 0x6985
const codeInvalidParam = 0x6b01

var errUserRejected = errors.New("user denied request")
var errInvalidParam = errors.New("invalid request parameters")

func (n *NanoS) Exchange(cmd byte, p1, p2 byte, data []byte) (resp []byte, err error) {
	resp, err = n.device.Exchange(APDU{
		CLA:     0xE0,
		INS:     cmd,
		P1:      p1,
		P2:      p2,
		Payload: data,
	})
	if err != nil {
		return nil, err
	} else if len(resp) < 2 {
		return nil, errors.New("APDU response missing status code")
	}
	// code := binary.BigEndian.Uint16(resp[len(resp)-2:])
	resp = resp[0 : len(resp)-2]
	// switch code {
	// case codeSuccess:
	// 	err = nil
	// case codeUserRejected:
	// 	err = errUserRejected
	// case codeInvalidParam:
	// 	err = errInvalidParam
	// default:
	// 	err = ErrCode(code)
	// }
	return
}

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
	cmdSignMetaData     = 0x12
	cmdGetOTAKey        = 0x13
	cmdEncryptCoin      = 0x50
	cmdDecryptCoin      = 0x51
	cmdGetValidatorKey  = 0x90
	cmdTrustDevice      = 0x60

	p1First = 0x00
	p1More  = 0x80

	p2DisplayAddress = 0x00
	p2DisplayPubkey  = 0x01
	p2DisplayHash    = 0x00
	p2SignHash       = 0x01
)

func (n *NanoS) GenKeyImage(kOTA []byte, coinPubKey []byte) ([]byte, error) {
	buf := new(bytes.Buffer)
	buf.Write(kOTA)
	buf.Write(coinPubKey)
	resp, err := n.Exchange(cmdKeyImage, 0, 0, buf.Next(255))
	if err != nil {
		return nil, err
	}
	return resp[:32], nil
}

func (n *NanoS) ImportPrivateKey(privateKey []byte) error {
	buf := new(bytes.Buffer)
	buf.Write(privateKey)
	resp, err := n.Exchange(cmdImportPrivateKey, 0, 0, buf.Next(255))
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

func (n *NanoS) TrustDevice() error {
	resp, err := n.Exchange(cmdTrustDevice, 0, 0, nil)
	if err != nil {
		return err
	}
	_ = resp
	return nil
}

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
			deviceIO: device,
		},
	}, nil
}
