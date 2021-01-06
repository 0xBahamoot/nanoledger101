package main

import (
	"errors"

	"github.com/incognitochain/incognito-chain/common"
	"github.com/incognitochain/incognito-chain/privacy/coin"
	"github.com/incognitochain/incognito-chain/privacy/key"
	"github.com/incognitochain/incognito-chain/privacy/operation"
)

func DecryptCoinsV2(coinList []*coin.CoinV2, viewKey key.ViewingKey, OTAKey key.OTAKey) error {
	for _, c := range coinList {
		txConcealRandomPoint, err := c.GetTxRandom().GetTxConcealRandomPoint()
		if err != nil {
			return err
		}
		rK := new(operation.Point).ScalarMult(txConcealRandomPoint, viewKey.GetPrivateView())

		// Hash multiple times
		hash := operation.HashToScalar(rK.ToBytesS())
		hash = operation.HashToScalar(hash.ToBytesS())
		randomness := c.GetRandomness().Sub(c.GetRandomness(), hash)

		// Hash 1 more time to get value
		hash = operation.HashToScalar(hash.ToBytesS())
		value := c.GetAmount().Sub(c.GetAmount(), hash)

		commitment := operation.PedCom.CommitAtIndex(value, randomness, operation.PedersenValueIndex)
		// for `confidential asset` coin, we commit differently
		if c.GetAssetTag() != nil {
			com, err := coin.ComputeCommitmentCA(c.GetAssetTag(), randomness, value)
			if err != nil {
				err := errors.New("Cannot recompute commitment when decrypting")
				return err
			}
			commitment = com
		}
		if !operation.IsPointEqual(commitment, c.GetCommitment()) {
			err := errors.New("Cannot Decrypt CoinV2: Commitment is not the same after decrypt")
			return err
		}
		c.SetRandomness(randomness)
		c.SetAmount(value)
	}
	if err := GetKeyImageOfCoins(coinList, OTAKey); err != nil {
		return err
	}
	return nil
}

func GetKeyImageOfCoins(coinList []*coin.CoinV2, OTAKey key.OTAKey) error {
	//GetCoinsImage
	for _, c := range coinList {
		_, txRandomOTAPoint, index, err := c.GetTxRandomDetail()
		if err != nil {
			return err
		}
		rK := new(operation.Point).ScalarMult(txRandomOTAPoint, OTAKey.GetOTASecretKey())  //(r_ota*G) * k = r_ota * K
		H := operation.HashToScalar(append(rK.ToBytesS(), common.Uint32ToBytes(index)...)) // Hash(r_ota*K, index)
		_ = H

		//send to ledger to continue
	}
	return nil
}
