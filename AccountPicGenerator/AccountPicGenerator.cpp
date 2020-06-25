// AccountPicGenerator.cpp : Defines the entry point for the application.
//

#include "AccountPicGenerator.h"

unsigned char header[28] = {
	// Size header (size - 4, size - 8)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// Header 
	0x31, 0x53, 0x50, 0x53,	0x18, 0xb0, 0x8b, 0x0b, 0x25, 0x27, 0x44, 0x4b,
	0x92, 0xba, 0x79, 0x33,	0xae, 0xb2, 0xdd, 0xe7
};

unsigned char meta1[45] = {
	// First "metadata" section
	0x04, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00,
	0x00, 0x70, 0x00, 0x72, 0x00, 0x6f, 0x00, 0x70, 0x00, 0x34, 0x00, 0x32,
	0x00, 0x39, 0x00, 0x34, 0x00, 0x39, 0x00, 0x36, 0x00, 0x37, 0x00, 0x32,
	0x00, 0x39, 0x00, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char middle[2] = {
	// padding
	0x00, 0x00, 
};

unsigned char meta2[45] = {
	// Second metadata section
	0x03, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00,
	0x00, 0x70, 0x00, 0x72, 0x00, 0x6f, 0x00, 0x70, 0x00, 0x34, 0x00, 0x32,
	0x00, 0x39, 0x00, 0x34, 0x00, 0x39, 0x00, 0x36, 0x00, 0x37, 0x00, 0x32,
	0x00, 0x39, 0x00, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char finale[8] = {
	// final padding
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int main(int argc, char** argv)
{
	if (argc != 4) {
		std::cerr << "Command usage: AccountPicGenerator.exe image-96.jpg image-448.jpg image.accountpicture-ms" << std::endl;
		return 1;
	}

	// Output image
	std::ofstream image_account(argv[3], std::ios::binary | std::ios::out);

	// Input image (96x96)
	std::ifstream image_96(argv[1], std::ios::binary | std::ios::in);
	
	// Input image (448x448)
	std::ifstream image_448(argv[2], std::ios::binary | std::ios::in);

	// Create string from files
	std::string data96((std::istreambuf_iterator<char>(image_96)), (std::istreambuf_iterator<char>()));
	std::string data448((std::istreambuf_iterator<char>(image_448)), (std::istreambuf_iterator<char>()));

	// Create string from metadata and headers
	std::string sheader(reinterpret_cast<char*>(header), sizeof(header));
	std::string smiddle(reinterpret_cast<char*>(middle), sizeof(middle));
	std::string smeta1(reinterpret_cast<char*>(meta1), sizeof(meta1));
	std::string sfinale(reinterpret_cast<char*>(finale), sizeof(finale));
	std::string smeta2(reinterpret_cast<char*>(meta2), sizeof(meta2));

	// Calculate sizes
	// All sizes are 4 bytes
	// first image size + metadata1 size + padding
	uint32_t s1 = (data96.length() + 55);
	uint32_t s2 = (data96.length());
	uint32_t s3 = (data448.length() + 53);
	uint32_t s4 = (data448.length());

	// Write header
	image_account << sheader;
	// Write first data section size
	image_account.write(reinterpret_cast<const char *>(&s1), sizeof(s1));
	// Write first "metadata section"
	image_account << smeta1;
	// Write first image size
	image_account.write(reinterpret_cast<const char*>(&s2), sizeof(s2));
	// Write image and middle padding
	image_account << data96 << smiddle;
	// Write second data section size
	image_account.write(reinterpret_cast<const char*>(&s3), sizeof(s3));
	// Write second metadata section
	image_account << smeta2;
	// Write second image size
	image_account.write(reinterpret_cast<const char*>(&s4), sizeof(s4));
	// Write second image and final padding
	image_account << data448 << sfinale;

	// Extract final size and first 8 bytes header
	uint32_t size = image_account.tellp();
	uint32_t si = size - 4;
	uint32_t sp = size - 8;

	// write "real" header
	image_account.seekp(0, std::ios::beg);
	image_account.write(reinterpret_cast<const char*>(&si), sizeof(si));
	image_account.write(reinterpret_cast<const char*>(&sp), sizeof(sp));

	return 0;
}
