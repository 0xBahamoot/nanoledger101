package main

func GetShardIDFromLastByte(b byte) byte {
	return byte(int(b) % 8)
}
