

class CIBitmap;

#include "tiffiop.h"

#define TIFF2PDF_MODULE "tiff2pdf"
#define T2P_VERSION "d"

class CPdf
{
public:

	/* This type is of PDF color spaces. */
	enum t2p_cs_t{
		T2P_CS_BILEVEL=0x01, /* Bilevel, black and white */
		T2P_CS_GRAY=0x02, /* Single channel */
		T2P_CS_RGB=0x04, /* Three channel tristimulus RGB */
		T2P_CS_LAB=0x10, /* Three channel L*a*b* color space */
		T2P_CS_PALETTE=0x1000 /* One of the above with a color map */
	} ;

	/* This type is of PDF compression types.  */
	enum t2p_compress_t{
		T2P_COMPRESS_NONE=0x00
		, T2P_COMPRESS_JPEG=0x02
		, T2P_COMPRESS_ZIP=0x04
	} ;

	/* This type is whether TIFF image data can be used in PDF without transcoding. */
	enum t2p_transcode_t{
		T2P_TRANSCODE_RAW=0x01, /* The raw data from the input can be used without recompressing */
		T2P_TRANSCODE_ENCODE=0x02 /* The data from the input is perhaps unencoded and reencoded */
	} ;

	/* This type is of information about the data samples of the input image. */
	enum t2p_sample_t{
		T2P_SAMPLE_NOTHING=0x0000, /* The unencoded samples are normal for the output colorspace */
		T2P_SAMPLE_ABGR_TO_RGB=0x0001, /* The unencoded samples are the result of ReadRGBAImage */
		T2P_SAMPLE_RGBA_TO_RGB=0x0002, /* The unencoded samples are contiguous RGBA */
		T2P_SAMPLE_RGBAA_TO_RGB=0x0004, /* The unencoded samples are RGBA with premultiplied alpha */
		T2P_SAMPLE_YCBCR_TO_RGB=0x0008, 
		T2P_SAMPLE_YCBCR_TO_LAB=0x0010, 
		T2P_SAMPLE_REALIZE_PALETTE=0x0020, /* The unencoded samples are indexes into the color map */
		T2P_SAMPLE_SIGNED_TO_UNSIGNED=0x0040, /* The unencoded samples are signed instead of unsignd */
		T2P_SAMPLE_LAB_SIGNED_TO_UNSIGNED=0x0040, /* The L*a*b* samples have a* and b* signed */
		T2P_SAMPLE_PLANAR_SEPARATE_TO_CONTIG=0x0100 /* The unencoded samples are separate instead of contiguous */
	} ;

	/* This type is of error status of the T2P struct. */
	enum t2p_err_t{
		T2P_ERR_OK = 0, /* This is the value of t2p->t2p_error when there is no error */
		T2P_ERR_ERROR = 1 /* This is the value of t2p->t2p_error when there was an error */
	};

	/* This struct defines a logical page of a TIFF. */
	typedef struct T2P_PAGE{
		tdir_t page_directory;
		uint32 page_number;
		ttile_t page_tilecount;
		uint32 page_extra;
	};

	/* This struct defines a PDF rectangle's coordinates. */
	typedef struct T2P_BOX{
		float x1;
		float y1;
		float x2;
		float y2;
		float mat[9];
	};

	/* This struct defines a tile of a PDF.  */
	typedef struct T2P_TILE{
		T2P_BOX tile_box;
	} ;

	/* This struct defines information about the tiles on a PDF page. */
	typedef struct T2P_TILES{
		ttile_t tiles_tilecount;
		uint32 tiles_tilewidth;
		uint32 tiles_tilelength;
		uint32 tiles_tilecountx;
		uint32 tiles_tilecounty;
		uint32 tiles_edgetilewidth;
		uint32 tiles_edgetilelength;
		T2P_TILE* tiles_tiles;
	} ;

	/* This struct is the context of a function to generate PDF from a TIFF. */
	typedef struct T2P{
		t2p_err_t t2p_error;
		T2P_PAGE* tiff_pages;
		T2P_TILES* tiff_tiles;
		tdir_t tiff_pagecount;
		uint16 tiff_compression;
		uint16 tiff_photometric;
		uint16 tiff_fillorder;
		uint16 tiff_bitspersample;
		uint16 tiff_samplesperpixel;
		uint16 tiff_planar;
		uint32 tiff_width;
		uint32 tiff_length;
		float tiff_xres;
		float tiff_yres;
		uint16 tiff_orientation;
		toff_t tiff_dataoffset;
		tsize_t tiff_datasize;
		TIFFReadWriteProc tiff_readproc;
		TIFFReadWriteProc tiff_writeproc;
		TIFFSeekProc tiff_seekproc;
		uint16 tiff_resunit;
		uint16 pdf_centimeters;
		uint16 pdf_overrideres;
		uint16 pdf_overridepagesize;
		float pdf_defaultxres;
		float pdf_defaultyres;
		float pdf_xres;
		float pdf_yres;
		float pdf_defaultpagewidth;
		float pdf_defaultpagelength;
		float pdf_pagewidth;
		float pdf_pagelength;
		float pdf_imagewidth;
		float pdf_imagelength;
		T2P_BOX pdf_mediabox;
		T2P_BOX pdf_imagebox;
		uint16 pdf_majorversion;
		uint16 pdf_minorversion;
		uint32 pdf_catalog;
		uint32 pdf_pages;
		uint32 pdf_info;
		uint32 pdf_palettecs;
		uint16 pdf_fitwindow;
		uint32 pdf_startxref;
		char* pdf_fileid;
		char* pdf_datetime;
		char* pdf_creator;
		char* pdf_author;
		char* pdf_title;
		char* pdf_subject;
		char* pdf_keywords;
		t2p_cs_t pdf_colorspace;
		uint16 pdf_colorspace_invert;
		uint16 pdf_switchdecode;
		uint16 pdf_palettesize;
		unsigned char* pdf_palette;
		int pdf_labrange[4];
		t2p_compress_t pdf_defaultcompression;
		uint16 pdf_defaultcompressionquality;
		t2p_compress_t pdf_compression;
		uint16 pdf_compressionquality;
		uint16 pdf_nopassthrough;
		t2p_transcode_t pdf_transcode;
		t2p_sample_t pdf_sample;
		uint32* pdf_xrefoffsets;
		uint32 pdf_xrefcount;
		tdir_t pdf_page;
		uint16 tiff_transferfunctioncount;
	} ;

	CPdf(void);
	~CPdf(void);

	bool WritePDFImage(CIBitmap &m_ciBitmap, const char * filename,const int &iFormatCompress = 0,const int &iTauxCompress = 0);

private:

	t2p_sample_t pdf_sample;

	int tiff2pdf_match_paper_size(float* width, float* length, char* papersize);
	void t2p_validate(T2P* t2p);
	T2P * init();
	
	static tsize_t t2p_empty_readproc(thandle_t fd, tdata_t buf, tsize_t size);
	static tsize_t t2p_empty_writeproc(thandle_t fd, tdata_t buf, tsize_t size);
	static toff_t t2p_empty_seekproc(thandle_t fd, toff_t off, int whence);
	static int t2p_empty_closeproc(thandle_t fd);
	void t2p_free(T2P* t2p);
	tsize_t t2p_write_pdf(T2P* t2p, CIBitmap &m_ciBitmap, TIFF* output);
	void t2p_read_tiff_data(T2P* t2p, CIBitmap &m_ciBitmap);
	void t2p_read_tiff_init(T2P* t2p, CIBitmap &m_ciBitmap);
	void t2p_read_tiff_size(T2P* t2p, CIBitmap &m_ciBitmap);
	tsize_t t2p_write_pdf_header(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_obj_start(uint32 number, TIFF* output);
	tsize_t t2p_write_pdf_obj_end(TIFF* output);
	tsize_t t2p_write_pdf_name(char* name, TIFF* output);
	tsize_t t2p_write_pdf_string(char* pdfstr, TIFF* output);
	tsize_t t2p_write_pdf_catalog(T2P* t2p, TIFF* output);
	tsize_t t2p_readwrite_pdf_image(T2P* t2p, CIBitmap &m_ciBitmap, TIFF* output);
	tsize_t t2p_write_pdf_info(T2P* t2p, CIBitmap &m_ciBitmap, TIFF* output);
	void t2p_pdf_tifftime(T2P* t2p);
	tsize_t t2p_write_pdf_pages(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_page(uint32 object, T2P* t2p, TIFF* output);
	void t2p_pdf_currenttime(T2P* t2p);
	tsize_t t2p_write_pdf_stream_dict_start(TIFF* output);
	tsize_t t2p_write_pdf_stream_dict(tsize_t len, uint32 number, TIFF* output);
	tsize_t t2p_write_pdf_stream_dict_end(TIFF* output);
	tsize_t t2p_write_pdf_stream_start(TIFF* output);
	tsize_t t2p_write_pdf_page_content_stream(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_stream_end(TIFF* output);
	tsize_t t2p_write_pdf_stream_length(tsize_t len, TIFF* output);
	tsize_t t2p_write_pdf_xobject_stream_dict(ttile_t tile, T2P* t2p, TIFF* output);
	void t2p_write_advance_directory(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_xreftable(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_trailer(T2P* t2p, TIFF* output);
	void t2p_compose_pdf_page(T2P* t2p);
	tsize_t t2p_write_pdf_stream(tdata_t buffer, tsize_t len, TIFF* output);
	void  t2p_compose_pdf_page_orient(T2P_BOX* boxp, uint16 orientation);
	void t2p_compose_pdf_page_orient_flip(T2P_BOX* boxp, uint16 orientation);
	int t2p_tile_is_right_edge(T2P_TILES tiles, ttile_t tile);
	int t2p_tile_is_bottom_edge(T2P_TILES tiles, ttile_t tile);
	tsize_t t2p_write_pdf_xobject_cs(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_xobject_decode(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_xobject_stream_filter(ttile_t tile, T2P* t2p, TIFF* output);
	tsize_t t2p_sample_rgba_to_rgb(tdata_t data, uint32 samplecount);

	tsize_t  t2p_write_pdf_xobject_palettecs_stream(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_transfer(T2P* t2p, TIFF* output);
	tsize_t t2p_write_pdf_transfer_dict(T2P* t2p, TIFF* output, uint16 i);
	tsize_t t2p_write_pdf_transfer_stream(T2P* t2p, TIFF* output, uint16 i);
	int ConvertTiffToPdf(CIBitmap &m_ciBitmap, const char * cFileOut);
	T2P* t2p_init();

};
