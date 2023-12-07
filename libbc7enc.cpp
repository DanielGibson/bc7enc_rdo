
#include "libbc7enc.h"


#include <stdio.h>

enum bc7enc_error {
	bc7enc_error_success = 0,
	bc7enc_error_no_source_file_name,
	bc7enc_error_null_output_pointer,
	bc7enc_error_could_not_load_source_file,
	bc7enc_error_could_not_initialize_encoder,
	bc7enc_error_could_not_encode_image,
	bc7enc_error_out_of_memory,
	bc7enc_error_no_data,
};

// FIXME: Add alternative entry point that takes rgba image data directly.

enum bc7enc_error compress_image_from_memory(int width, int height, void* data, rdo_bc::rdo_bc_params params, encode_output* output)
{
	// If we don't 
	if (output == NULL)
		return bc7enc_error_null_output_pointer;

	if (data == NULL)
	{
		return bc7enc_error_no_data;
	}

	// FIXME: Avoid making a complete copy of the input image...
	utils::image_u8 source_image(width, height, (utils::color_quad_u8*)data);

	// FIXME: Make the encoder write directly to a specified memory location
	// We can't take ownership of the data inside the vector so as long as we use it
	// we need to do a copy of the data...
	rdo_bc::rdo_bc_encoder encoder;
	if (!encoder.init(source_image, params))
	{
		return bc7enc_error_could_not_initialize_encoder;
	}

	if (!encoder.encode())
	{
		return bc7enc_error_could_not_encode_image;
	}

	output->width = encoder.get_orig_width();
	output->height = encoder.get_orig_height();
	output->mipmap_count = encoder.get_mip_levels();
	output->format = params.m_dxgi_format;
	output->blocks = (char*)malloc(encoder.get_total_blocks_all_mips() * encoder.get_bytes_per_block());
	if (output->blocks == NULL)
	{
		return bc7enc_error_out_of_memory;
	}
	const void* blocks2 = encoder.get_blocks();
	memcpy(output->blocks, blocks2, encoder.get_total_blocks_all_mips_size_in_bytes());
	output->num_blocks = encoder.get_total_blocks_all_mips();
	output->bytes_per_block = encoder.get_bytes_per_block();

	return bc7enc_error_success;
}

enum bc7enc_error compress_image_from_file(char* image_path, rdo_bc::rdo_bc_params params, encode_output* output)
{
	// If we don't 
	if (output == NULL)
		return bc7enc_error_null_output_pointer;

	if (image_path == NULL || strlen(image_path) == 0)
	{
		return bc7enc_error_no_source_file_name;
	}

	utils::image_u8 source_image;
	if (!load_png(image_path, source_image))
		return bc7enc_error_could_not_load_source_file;

	// FIXME: Make the encoder write directly to a specified memory location
	// We can't take ownership of the data inside the vector so as long as we use it
	// we need to do a copy of the data...
	rdo_bc::rdo_bc_encoder encoder;
	if (!encoder.init(source_image, params))
	{
		return bc7enc_error_could_not_initialize_encoder;
	}

	if (!encoder.encode())
	{
		return bc7enc_error_could_not_encode_image;
	}

	output->width = encoder.get_orig_width();
	output->height = encoder.get_orig_height();
	output->mipmap_count = encoder.get_mip_levels();
	output->format = params.m_dxgi_format;
	output->blocks = (char*)malloc(encoder.get_total_blocks_all_mips() * encoder.get_bytes_per_block());
	if (output->blocks == NULL)
	{
		return bc7enc_error_out_of_memory;
	}
	const void* blocks2 = encoder.get_blocks();
	memcpy(output->blocks, blocks2, encoder.get_total_blocks_all_mips_size_in_bytes());
	output->num_blocks = encoder.get_total_blocks_all_mips();
	output->bytes_per_block = encoder.get_bytes_per_block();

	return bc7enc_error_success;
}

void free_encode_output(encode_output* output)
{
	free(output->blocks);
	output->blocks = NULL;
}