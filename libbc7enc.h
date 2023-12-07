
#pragma once

#include "rdo_bc_encoder.h"

#if defined(_WIN64)
#define EXPORT __declspec(dllexport)
#elif defined(__linux__)
#define EXPORT __attribute__((visibility("default"))) 
#endif

extern "C" {

	typedef struct encode_output {
		int width;
		int height;
		int mipmap_count;
		DXGI_FORMAT format;
		char* blocks;
		int num_blocks;
		int bytes_per_block;
	} encode_output;

	EXPORT enum bc7enc_error compress_image_from_memory(int width, int height, void* data, rdo_bc::rdo_bc_params params, encode_output* output);

	EXPORT enum bc7enc_error compress_image_from_file(char* image_path, rdo_bc::rdo_bc_params params, encode_output* output);

	EXPORT void free_encode_output(encode_output* output);

}

