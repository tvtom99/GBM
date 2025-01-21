#pragma once

struct keys1
{
    unsigned char start : 1; // adding a tailing ': 1' defines this as only contaning a single bit
    unsigned char select : 1;
    unsigned char b : 1;
    unsigned char a : 1;

    // Because I am defining individual bits here, I have to consider if it is big or little endian.
    // By default windows is big endian (most significant bits on the left), so it has this order.
};

// Same deal below as above, but this is the array for if reading dpad
struct keys2
{
    unsigned char down : 1;
    unsigned char up : 1;
    unsigned char left : 1;
    unsigned char right : 1;
};

// This was taken from Cinoop directly, but it just makes so much sense ;-;
/*
    keys.c - full access to the byte holding the joypad information
    keys.key1 - first 4 bits of keys byte
    keys.key2 - second 4 bits of keys byte

*/
struct keys {
	union {
		struct {
			union {
				struct keys1;
				unsigned char keys1 : 4;
			};
			
			union {
				struct keys2;
				unsigned char keys2 : 4;
			};
		};
		
		unsigned char c;
	};
} extern keys;