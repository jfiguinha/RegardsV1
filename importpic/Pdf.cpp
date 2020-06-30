#include "stdafx.h"
#include ".\pdf.h"
#include <time.h>

CPdf::CPdf(void)
{
}

CPdf::~CPdf(void)
{
}

/*
	This function allocates and initializes a T2P context struct pointer.
*/

CPdf::T2P* CPdf::t2p_init(){

	T2P* t2p = (T2P*) _TIFFmalloc(sizeof(T2P));
	if(t2p==NULL){
		TIFFError(
			TIFF2PDF_MODULE, 
			"Can't allocate %u bytes of memory for t2p_init", 
			sizeof(T2P));
		return( (T2P*) NULL );
	}
	_TIFFmemset(t2p, 0x00, sizeof(T2P));
	t2p->pdf_majorversion=1;
	t2p->pdf_minorversion=1;
	t2p->pdf_defaultxres=300.0;
	t2p->pdf_defaultyres=300.0;
	t2p->pdf_defaultpagewidth=612.0;
	t2p->pdf_defaultpagelength=792.0;
	t2p->pdf_xrefcount=3; /* Catalog, Info, Pages */
	
	return(t2p);
}

int CPdf::ConvertTiffToPdf(CIBitmap &m_ciBitmap, const char * cFileOut)
{
	//extern int optind;
	//extern char *optarg;
	const char * m_cAutor = "Figuinha Jacques";
	const char * m_cCreator = "Figuinha Jacques";
	const char * m_cTitle = "Regards";
	T2P *t2p = NULL;
	TIFF *output = NULL;
	tsize_t written=0;
	int c=0;
	int i = 0;
	
	t2p = t2p_init();

	if (t2p == NULL){
		TIFFError(
			TIFF2PDF_MODULE, 
			"Can't initialize context");
		goto failexit;
	}

	t2p->pdf_defaultcompression=T2P_COMPRESS_NONE;

	t2p->pdf_creator = (char *)_TIFFmalloc(strlen(m_cCreator) + 1);
	strcpy(t2p->pdf_creator, m_cCreator);
	t2p->pdf_creator[strlen(m_cCreator)]=0;

	t2p->pdf_author = (char *)_TIFFmalloc(strlen(m_cAutor) + 1);
	strcpy(t2p->pdf_author, m_cAutor);
	t2p->pdf_author[strlen(m_cAutor)]=0;

	t2p->pdf_title= (char*)_TIFFmalloc(strlen(m_cTitle)+1);
	strcpy(t2p->pdf_title, m_cTitle);
	t2p->pdf_title[strlen(m_cTitle)]=0;

	t2p->pdf_subject= (char*)_TIFFmalloc(strlen(m_cTitle)+1);
	strcpy(t2p->pdf_subject, m_cTitle);
	t2p->pdf_subject[strlen(m_cTitle)]=0;

	if (cFileOut) {
		output = TIFFOpen(cFileOut, "w");
		if(output == NULL) {
			goto failfreet2p;
		}
		if(output->tif_seekproc != NULL) {
			TIFFSeekFile(output, (toff_t) 0, SEEK_SET);
		}
	} 

	written = t2p_write_pdf(t2p, m_ciBitmap, output);

	if(output != NULL){
		TIFFClose(output);
	}
	if(t2p != NULL){
		t2p_free(t2p);
	}

	return(EXIT_SUCCESS);


failfreet2p:
	if(t2p != NULL){
		t2p_free(t2p);
	}
failexit:
	return(EXIT_FAILURE);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool CPdf::WritePDFImage(CIBitmap &m_ciBitmap, const char * filename,const int &iFormatCompress,const int &iTauxCompress)
{
	return ConvertTiffToPdf(m_ciBitmap,filename);

	int optind;
	T2P* t2p = NULL;
	TIFF* output = NULL;
	tsize_t written=0;
	int c=0;

	////////////////////////////////////////////////
	//Liste des parametres
	////////////////////////////////////////////////

	string m_stgAuteur = "Figuinha Jacques";
	string m_stgCreateur = "Figuinha Jacques";
	string m_stgSujet = filename;
	string m_stgDateTime = "";
	string m_stgTitle = filename;
	string m_stgKeyWords = "";

	int iCentimetre, iResolution, iColorInverse, iFitToWindows;
	int iSelectFormat = 2;
	int iOverRide;
	float fWidth, fLength;
	string m_stgFormat[3] = {"letter","legal","A4"};

	////////////////////////////////////////////////
	//Initialisation des valeurs
	////////////////////////////////////////////////

	iCentimetre = 1;
	iResolution = 72;
	iColorInverse = 0;
	iFitToWindows = 0;
	iOverRide = 0;
	fWidth = 0.0;
	fLength = 0.0;

	////////////////////////////////////////////////
	////////////////////////////////////////////////
	
	t2p = init();

	if (t2p == NULL)
	{
		goto failexit;
	}

	////////////////////////////////////////////////////
	//Creation du fichier de sortie
	////////////////////////////////////////////////////

	output=TIFFOpen(filename, "w");
	if(output==NULL)
	{
		goto failfreet2p;
	}

	if(output->tif_seekproc != NULL)
	{
		TIFFSeekFile(output, (toff_t) 0, SEEK_SET);
	}

	//Choix du format de compression JPEG ou Zip

	switch(iFormatCompress)
	{
		case 1:
			{
				t2p->pdf_defaultcompression=T2P_COMPRESS_JPEG;
				//Selection du taux de compression
				t2p->pdf_defaultcompressionquality=iTauxCompress;
				break;
			}
		case 2:
			{
				t2p->pdf_defaultcompressionquality=iTauxCompress;
				t2p->pdf_defaultcompression=T2P_COMPRESS_ZIP;
				break;
			}
			
		default:
			//Pas de compression
			t2p->pdf_defaultcompression=T2P_COMPRESS_NONE;
			t2p->pdf_nopassthrough=1;
			break;
	}
		
	//Choix du standart m ou inch
/*
	//Par défault le metre
	if(iCentimetre)
		t2p->pdf_centimeters=1;


	
	//Definition de l'image
	t2p->pdf_defaultxres = iResolution / (t2p->pdf_centimeters?2.54:1.0);
	t2p->pdf_defaultyres = iResolution / (t2p->pdf_centimeters?2.54:1.0);

	if(iOverRide)
	{
		t2p->pdf_overridepagesize=1;
		t2p->pdf_overrideres=1;
		t2p->pdf_defaultpagewidth = (fWidth * 72) / (t2p->pdf_centimeters?2.54:1.0);
		t2p->pdf_defaultpagelength = (fLength * 72) / (t2p->pdf_centimeters?2.54:1.0);
	}*/

	///////////////////////////////////////////////////////////
	//Format du papier
	///////////////////////////////////////////////////////////

	/*
	if(tiff2pdf_match_paper_size(&(t2p->pdf_defaultpagewidth), &(t2p->pdf_defaultpagelength), m_stgFormat[iSelectFormat].c_str()))
	{
		t2p->pdf_overridepagesize=1;
	}*/


	///////////////////////////////////////////////////////////
	//Inversion des couleurs
	///////////////////////////////////////////////////////////

	if(iColorInverse)
	{
		t2p->pdf_colorspace_invert=1;
	}

	///////////////////////////////////////////////////////////
	//Fit to Window
	///////////////////////////////////////////////////////////

	if(iFitToWindows) 
	{
		t2p->pdf_fitwindow=1;
	}

	///////////////////////////////////////////////////////////
	//Date de creation
	///////////////////////////////////////////////////////////

	t2p->pdf_datetime= (char*)_TIFFmalloc(17);
	if(t2p->pdf_datetime==NULL)
	{
		goto failcloseoutput;
	}

	t2p->pdf_datetime[0]=0;


	///////////////////////////////////////////////////////////
	//Createur
	///////////////////////////////////////////////////////////

	t2p->pdf_creator= (char*)_TIFFmalloc(m_stgCreateur.length()+1);
	if(t2p->pdf_creator==NULL)
	{
		goto failcloseoutput;
	}
	strcpy(t2p->pdf_creator, m_stgCreateur.c_str());
	t2p->pdf_creator[m_stgCreateur.length()]=0;

	///////////////////////////////////////////////////////////
	//Auteur
	///////////////////////////////////////////////////////////

	t2p->pdf_author= (char*)_TIFFmalloc(m_stgAuteur.length()+1);
	if(t2p->pdf_author==NULL)
	{
		goto failcloseoutput;
	}
	strcpy(t2p->pdf_author, m_stgAuteur.c_str());
	t2p->pdf_author[m_stgAuteur.length()]=0;

	///////////////////////////////////////////////////////////
	//Titre
	///////////////////////////////////////////////////////////

	t2p->pdf_title= (char*)_TIFFmalloc(m_stgTitle.length()+1);
	if(t2p->pdf_title==NULL)
	{
		goto failcloseoutput;
	}
	strcpy(t2p->pdf_title, m_stgTitle.c_str());
	t2p->pdf_title[m_stgTitle.length()]=0;

	///////////////////////////////////////////////////////////
	//Sujet
	///////////////////////////////////////////////////////////

	t2p->pdf_subject= (char*)_TIFFmalloc(m_stgSujet.length()+1);
	if(t2p->pdf_subject==NULL)
	{
		goto failcloseoutput;
	}
	strcpy(t2p->pdf_subject, m_stgSujet.c_str());
	t2p->pdf_subject[m_stgSujet.length()]=0;

	/////////////////////////////////////////////////////////
	//Mots clés
	/////////////////////////////////////////////////////////

	t2p->pdf_keywords= (char*)_TIFFmalloc(m_stgKeyWords.length()+1);
	if(t2p->pdf_keywords==NULL)
	{
		goto failcloseoutput;
	}
	strcpy(t2p->pdf_keywords, m_stgKeyWords.c_str());
	t2p->pdf_keywords[m_stgKeyWords.length()]=0;

	t2p_validate(t2p);

	if(output==NULL)
	{
		//Création du fichier PDF

		char temppath[MAX_PATH];
		char tempfile[MAX_PATH];
		GetTempPath((DWORD)MAX_PATH, (LPTSTR)temppath);
		GetTempFileName((LPCTSTR)temppath, (LPTSTR) TEXT("t2p"), 0, (LPTSTR)tempfile);
		output = TIFFFdOpen( (int)CreateFile((LPCTSTR)tempfile, GENERIC_WRITE, 
			0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE,  NULL), "-", "w");

		if(output==NULL)
		{
			goto failfreet2p;
		}

		TIFFFlush(output);
		output->tif_readproc=t2p_empty_readproc;
		output->tif_seekproc=t2p_empty_seekproc;
		output->tif_closeproc=t2p_empty_closeproc;

		CloseHandle((HANDLE) output->tif_fd);
		output->tif_fd=(int)GetStdHandle(STD_OUTPUT_HANDLE);
		output->tif_clientdata=(thandle_t)output->tif_fd;
	}

	written = t2p_write_pdf(t2p, m_ciBitmap, output);

	if(t2p->t2p_error != 0)
	{
		goto failfreet2p;
	}

	if(output != NULL){
		TIFFClose(output);
	}
	if(t2p != NULL)
	{
		t2p_free(t2p);
	}

	return(EXIT_SUCCESS);


failcloseoutput:

	if(output != NULL)
	{
		if(strcmp(TIFFFileName(output), "-") != 0)
		{
			unlink(TIFFFileName(output));
		}
		TIFFClose(output);
	}

failfreet2p:

	if(t2p != NULL)
	{
		t2p_free(t2p);
	}


failexit:
	return(EXIT_FAILURE);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int CPdf::tiff2pdf_match_paper_size(float* width, float* length, char* papersize)
{

	int i=0;
	int len=0;
	const char* sizes[]={
		"LETTER", "A4", "LEGAL",
		"EXECUTIVE", "LETTER", "LEGAL", "LEDGER", "TABLOID", 
		"A", "B", "C", "D", "E", "F", "G", "H", "J", "K", 
		"A10", "A9", "A8", "A7", "A6", "A5", "A4", "A3", "A2", "A1", "A0", 
		"2A0", "4A0", "2A", "4A", 
		"B10", "B9", "B8", "B7", "B6", "B5", "B4", "B3", "B2", "B1", "B0", 
		"JISB10", "JISB9", "JISB8", "JISB7", "JISB6", "JISB5", "JISB4", 
		"JISB3", "JISB2", "JISB1", "JISB0", 
		"C10", "C9", "C8", "C7", "C6", "C5", "C4", "C3", "C2", "C1", "C0", 
		"RA2", "RA1", "RA0", "SRA4", "SRA3", "SRA2", "SRA1", "SRA0", 
		"A3EXTRA", "A4EXTRA", 
		"STATEMENT", "FOLIO", "QUARTO", 
		NULL
	} ;
	const int widths[]={
		612, 595, 612,
		522, 612,612,792,792,
		612,792,1224,1584,2448,2016,792,2016,2448,2880,
		74,105,147,210,298,420,595,842,1191,1684,2384,3370,4768,3370,4768,
		88,125,176,249,354,499,709,1001,1417,2004,2835,
		91,128,181,258,363,516,729,1032,1460,2064,2920,
		79,113,162,230,323,459,649,918,1298,1298,2599,
		1219,1729,2438,638,907,1276,1814,2551,
		914,667,
		396, 612, 609, 
		0
	};
	const int lengths[]={
		792,842,1008,
		756,792,1008,1224,1224,
		792,1224,1584,2448,3168,2880,6480,10296,12672,10296,
		105,147,210,298,420,595,842,1191,1684,2384,3370,4768,6741,4768,6741,
		125,176,249,354,499,709,1001,1417,2004,2835,4008,
		128,181,258,363,516,729,1032,1460,2064,2920,4127,
		113,162,230,323,459,649,918,1298,1837,1837,3677,
		1729,2438,3458,907,1276,1814,2551,3628,
		1262,914,
		612, 936, 780, 
		0
	};

	len=strlen(papersize);
	for(i=0;i<len;i++){
		papersize[i]=toupper(papersize[i]);
	}
	for(i=0;sizes[i]!=NULL; i++){
		if (strcmp( (const char*)papersize, sizes[i])==0){
			*width=(float)widths[i];
			*length=(float)lengths[i];
			return(1);
		}
	}

	return(0);
}

/*
	This function allocates and initializes a T2P context struct pointer.
*/

CPdf::T2P * CPdf::init()
{

	T2P* t2p = (T2P*) _TIFFmalloc(sizeof(T2P));
	_TIFFmemset(t2p, 0x00, sizeof(T2P));
	if(t2p==NULL)
	{
		return( (T2P*) NULL );
	}
	t2p->pdf_majorversion=1;
	t2p->pdf_minorversion=1;
	t2p->pdf_defaultxres=300.0;
	t2p->pdf_defaultyres=300.0;
	t2p->pdf_defaultpagewidth=612.0;
	t2p->pdf_defaultpagelength=792.0;
	t2p->pdf_xrefcount=3; /* Catalog, Info, Pages */
	
	return(t2p);
}

/*
	This function validates the values of a T2P context struct pointer before calling 
	t2p_write_pdf with it.
*/

void CPdf::t2p_validate(T2P* t2p)
{

	if(t2p->pdf_defaultcompression==T2P_COMPRESS_JPEG)
	{
		if(t2p->pdf_defaultcompressionquality<100 || t2p->pdf_defaultcompressionquality<1)
		{
			t2p->pdf_defaultcompressionquality=0;
		}
	}

	if(t2p->pdf_defaultcompression==T2P_COMPRESS_ZIP)
	{
		switch (t2p->pdf_defaultcompressionquality)
		{
			case 1: case 10: case 11: case 12: case 13: case 14: case 15:
			case 101: case 110: case 111: case 112: case 113: case 114: case 115:
			case 201: case 210: case 211: case 212: case 213: case 214: case 215:
			case 301: case 310: case 311: case 312: case 313: case 314: case 315:
			case 401: case 410: case 411: case 412: case 413: case 414: case 415:
			case 501: case 510: case 511: case 512: case 513: case 514: case 515:
			case 601: case 610: case 611: case 612: case 613: case 614: case 615:
			case 701: case 710: case 711: case 712: case 713: case 714: case 715:
			case 801: case 810: case 811: case 812: case 813: case 814: case 815:
			case 901: case 910: case 911: case 912: case 913: case 914: case 915:
				break;
			default:
				t2p->pdf_defaultcompressionquality=0;
		}
		if(t2p->pdf_defaultcompressionquality%100 !=0)
		{
			
		}
		t2p->pdf_defaultcompressionquality%=100;
		if(t2p->pdf_minorversion<2)
		{
			t2p->pdf_minorversion=2;
		}
	}

	(void)0;

	return;
}

tsize_t CPdf::t2p_empty_readproc(thandle_t fd, tdata_t buf, tsize_t size)
{

	(void) fd; (void) buf; (void) size;

	return (size);
}

/*
	This function is an empty (dummy) TIFFReadWriteProc that returns the amount 
	requested to be written without writing anything.
*/

tsize_t CPdf::t2p_empty_writeproc(thandle_t fd, tdata_t buf, tsize_t size)
{

	(void) fd; (void) buf; (void) size;

	return (size);
}

/*
	This function is an empty (dummy) TIFFSeekProc that returns off.
*/

toff_t CPdf::t2p_empty_seekproc(thandle_t fd, toff_t off, int whence)
{
	
	(void) fd; (void) off; (void) whence;

	return( off );
}

/*
	This function is an empty (dummy) TIFFCloseProc that returns 0.
*/

int CPdf::t2p_empty_closeproc(thandle_t fd)
{
	
	(void) fd;
	
	return(0);
}

/*
	This function frees a T2P context struct pointer and any allocated data fields of it.
*/

void CPdf::t2p_free(T2P* t2p)
{

	int i=0;

	if(t2p != NULL){
		if(t2p->pdf_xrefoffsets != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_xrefoffsets);
		}
		if(t2p->tiff_pages != NULL){
			_TIFFfree( (tdata_t) t2p->tiff_pages);
		}
		for(i=0;i<t2p->tiff_pagecount;i++){
			if(t2p->tiff_tiles[i].tiles_tiles != NULL){
				_TIFFfree( (tdata_t) t2p->tiff_tiles[i].tiles_tiles);
			}
		}
		if(t2p->tiff_tiles != NULL){
			_TIFFfree( (tdata_t) t2p->tiff_tiles);
		}
		if(t2p->pdf_palette != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_palette);
		}
		if(t2p->pdf_fileid != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_fileid);
		}
		if(t2p->pdf_datetime != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_datetime);
		}
		if(t2p->pdf_creator != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_creator);
		}
		if(t2p->pdf_author != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_author);
		}
		if(t2p->pdf_title != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_title);
		}
		if(t2p->pdf_subject != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_subject);
		}
		if(t2p->pdf_keywords != NULL){
			_TIFFfree( (tdata_t) t2p->pdf_keywords);
		}
		_TIFFfree( (tdata_t) t2p );
	}

	return;
}

tsize_t CPdf::t2p_write_pdf_transfer(T2P* t2p, TIFF* output){

	tsize_t written=0;
	char buffer[16];
	int buflen=0;

	written += TIFFWriteFile(output, (tdata_t) "<< /Type /ExtGState \r/TR ", 25);
	if(t2p->tiff_transferfunctioncount == 1){
		buflen=sprintf(buffer, "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 1));
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " 0 R ", 5);
	} else {
		written += TIFFWriteFile(output, (tdata_t) "[ ", 2);
		buflen=sprintf(buffer, "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 1));
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " 0 R ", 5);
		buflen=sprintf(buffer, "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 2));
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " 0 R ", 5);
		buflen=sprintf(buffer, "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 3));
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " 0 R ", 5);
		written += TIFFWriteFile(output, (tdata_t) "/Identity ] ", 12);
	}

	written += TIFFWriteFile(output, (tdata_t) " >> \r", 5);

	return(written);
}

/*

  This function writes a PDF to a file given a pointer to a TIFF.

  The idea with using a TIFF* as output for a PDF file is that the file 
  can be created with TIFFClientOpen for memory-mapped use within the TIFF 
  library, and TIFFWriteEncodedStrip can be used to write compressed data to 
  the output.  The output is not actually a TIFF file, it is a PDF file.  

  This function uses only TIFFWriteFile and TIFFWriteEncodedStrip to write to 
  the output TIFF file.  When libtiff would otherwise be writing data to the 
  output file, the write procedure of the TIFF structure is replaced with an 
  empty implementation.

  The first argument to the function is an initialized and validated T2P 
  context struct pointer.

  The second argument to the function is the TIFF* that is the input that has 
  been opened for reading and no other functions have been called upon it.

  The third argument to the function is the TIFF* that is the output that has 
  been opened for writing.  It has to be opened so that it hasn't written any 
  data to the output.  If the output is seekable then it's OK to seek to the 
  beginning of the file.  The function only writes to the output PDF and does 
  not seek.  See the example usage in the main() function.

	TIFF* output = TIFFOpen("output.pdf", "w");
	assert(output != NULL);

	if(output->tif_seekproc != NULL){
		TIFFSeekFile(output, (toff_t) 0, SEEK_SET);
	}

  This function returns the file size of the output PDF file.  On error it 
  returns zero and the t2p->t2p_error variable is set to T2P_ERR_ERROR.

  After this function completes, call t2p_free on t2p, TIFFClose on input, 
  and TIFFClose on output.
*/

/*
	This function sets the input directory to the directory of a given
	page and determines information about the image.  It checks
	the image characteristics to determine if it is possible to convert
	the image data into a page of PDF output, setting values of the T2P
	struct for this page.  It determines what color space is used in
	the output PDF to represent the image.
	
	It determines if the image can be converted as raw data without
	requiring transcoding of the image data.
*/


tsize_t CPdf::t2p_write_pdf(T2P* t2p, CIBitmap &m_ciBitmap, TIFF* output)
{

	/*
	tsize_t written=0;
	ttile_t i2=0;
	tsize_t streamlen=0;


	t2p_read_tiff_init(t2p, m_ciBitmap);
	if(t2p->t2p_error!=T2P_ERR_OK)
	{
		return(0);
	}

	t2p->pdf_xrefoffsets= (uint32*) _TIFFmalloc(t2p->pdf_xrefcount * sizeof(uint32) );
	if(t2p->pdf_xrefoffsets==NULL)
	{
		return(written);
	}
	t2p->pdf_xrefcount=0;
	t2p->pdf_catalog=1;
	t2p->pdf_info=2;
	t2p->pdf_pages=3;
	written += t2p_write_pdf_header(t2p, output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_catalog=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_catalog(t2p, output);
	written += t2p_write_pdf_obj_end(output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_info=t2p->pdf_xrefcount;

	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_info(t2p, m_ciBitmap, output);
	written += t2p_write_pdf_obj_end(output);

	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_pages=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_pages(t2p, output);
	written += t2p_write_pdf_obj_end(output);

	for(t2p->pdf_page=0;t2p->pdf_page<t2p->tiff_pagecount;t2p->pdf_page++)
	{
		t2p_read_tiff_data(t2p, m_ciBitmap);
		if(t2p->t2p_error!=T2P_ERR_OK)
		{
			return(0);
		}
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_page(t2p->pdf_xrefcount, t2p, output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_dict_start(output);
		written += t2p_write_pdf_stream_dict(0, t2p->pdf_xrefcount+1, output);
		written += t2p_write_pdf_stream_dict_end(output);
		written += t2p_write_pdf_stream_start(output);
		streamlen=written;
		written += t2p_write_pdf_page_content_stream(t2p, output);
		streamlen=written-streamlen;
		written += t2p_write_pdf_stream_end(output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_length(streamlen, output);
		written += t2p_write_pdf_obj_end(output);
	
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_dict_start(output);
		written += t2p_write_pdf_xobject_stream_dict(0, t2p, output);
		written += t2p_write_pdf_stream_dict_end(output);
		written += t2p_write_pdf_stream_start(output);
		streamlen=written;

		t2p_read_tiff_size(t2p, m_ciBitmap);
		written += t2p_readwrite_pdf_image(t2p, m_ciBitmap, output);
		t2p_write_advance_directory(t2p, output);
		if(t2p->t2p_error!=T2P_ERR_OK)
		{
			return(0);
		}
		streamlen=written-streamlen;
		written += t2p_write_pdf_stream_end(output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_length(streamlen, output);
		written += t2p_write_pdf_obj_end(output);

	}

	t2p->pdf_startxref=written;
	written += t2p_write_pdf_xreftable(t2p, output);
	written += t2p_write_pdf_trailer(t2p, output);
	t2p->tiff_writeproc=output->tif_writeproc;
	output->tif_writeproc=t2p_empty_writeproc;

	return(written);
	*/

	tsize_t written=0;
	ttile_t i2=0;
	tsize_t streamlen=0;
	uint16 i=0;

	//t2p_read_tiff_init(t2p, input);
	t2p_read_tiff_init(t2p, m_ciBitmap);
	if(t2p->t2p_error!=T2P_ERR_OK){return(0);}
	t2p->pdf_xrefoffsets= (uint32*) _TIFFmalloc(t2p->pdf_xrefcount * sizeof(uint32) );
	if(t2p->pdf_xrefoffsets==NULL){
		TIFFError(
			TIFF2PDF_MODULE, 
			"Can't allocate %lu bytes of memory for t2p_write_pdf", 
			t2p->pdf_xrefcount * sizeof(uint32) );
		return(written);
	}
	t2p->pdf_xrefcount=0;
	t2p->pdf_catalog=1;
	t2p->pdf_info=2;
	t2p->pdf_pages=3;
	written += t2p_write_pdf_header(t2p, output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_catalog=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_catalog(t2p, output);
	written += t2p_write_pdf_obj_end(output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_info=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_info(t2p, m_ciBitmap, output);
	written += t2p_write_pdf_obj_end(output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_pages=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_pages(t2p, output);
	written += t2p_write_pdf_obj_end(output);
	for(t2p->pdf_page=0;t2p->pdf_page<t2p->tiff_pagecount;t2p->pdf_page++)
	{
		t2p_read_tiff_data(t2p, m_ciBitmap);
		if(t2p->t2p_error!=T2P_ERR_OK){return(0);}
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_page(t2p->pdf_xrefcount, t2p, output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_dict_start(output);
		written += t2p_write_pdf_stream_dict(0, t2p->pdf_xrefcount+1, output);
		written += t2p_write_pdf_stream_dict_end(output);
		written += t2p_write_pdf_stream_start(output);
		streamlen=written;
		written += t2p_write_pdf_page_content_stream(t2p, output);
		streamlen=written-streamlen;
		written += t2p_write_pdf_stream_end(output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_length(streamlen, output);
		written += t2p_write_pdf_obj_end(output);
		if(t2p->tiff_transferfunctioncount != 0){
			t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
			written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
			written += t2p_write_pdf_transfer(t2p, output);
			written += t2p_write_pdf_obj_end(output);
			for(i=0; i < t2p->tiff_transferfunctioncount; i++){
				t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
				written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
				written += t2p_write_pdf_stream_dict_start(output);
				written += t2p_write_pdf_transfer_dict(t2p, output, i);
				written += t2p_write_pdf_stream_dict_end(output);
				written += t2p_write_pdf_stream_start(output);
				streamlen=written;
				written += t2p_write_pdf_transfer_stream(t2p, output, i);
				streamlen=written-streamlen;
				written += t2p_write_pdf_stream_end(output);
				written += t2p_write_pdf_obj_end(output);
			}
		}
		if( (t2p->pdf_colorspace & T2P_CS_PALETTE) != 0){
			t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
			t2p->pdf_palettecs=t2p->pdf_xrefcount;
			written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
			written += t2p_write_pdf_stream_dict_start(output);
			written += t2p_write_pdf_stream_dict(t2p->pdf_palettesize, 0, output);
			written += t2p_write_pdf_stream_dict_end(output);
			written += t2p_write_pdf_stream_start(output);
			streamlen=written;
			written += t2p_write_pdf_xobject_palettecs_stream(t2p, output);
			streamlen=written-streamlen;
			written += t2p_write_pdf_stream_end(output);
			written += t2p_write_pdf_obj_end(output);
		}


		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_dict_start(output);
		written += t2p_write_pdf_xobject_stream_dict(
			0, 
			t2p, 
			output);
		written += t2p_write_pdf_stream_dict_end(output);
		written += t2p_write_pdf_stream_start(output);
		streamlen=written;
		t2p_read_tiff_size(t2p, m_ciBitmap);
		written += t2p_readwrite_pdf_image(t2p, m_ciBitmap, output);
		t2p_write_advance_directory(t2p, output);
		if(t2p->t2p_error!=T2P_ERR_OK){return(0);}
		streamlen=written-streamlen;
		written += t2p_write_pdf_stream_end(output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_length(streamlen, output);
		written += t2p_write_pdf_obj_end(output);

	}
	t2p->pdf_startxref=written;
	written += t2p_write_pdf_xreftable(t2p, output);
	written += t2p_write_pdf_trailer(t2p, output);

	return(written);
}

tsize_t CPdf::t2p_write_pdf_transfer_stream(T2P* t2p, TIFF* output, uint16 i){

	tsize_t written=0;

	/*
	written += t2p_write_pdf_stream(
		t2p->tiff_transferfunction[i], 
		(1<<(t2p->tiff_bitspersample+1)), 
		output);*/

	return(written);
}

tsize_t CPdf::t2p_write_pdf_transfer_dict(T2P* t2p, TIFF* output, uint16 i){

	tsize_t written=0;
	char buffer[32];
	int buflen=0;
	(void)i; // XXX

	written += TIFFWriteFile(output, (tdata_t) "/FunctionType 0 \r", 17);
	written += TIFFWriteFile(output, (tdata_t) "/Domain [0.0 1.0] \r", 19);
	written += TIFFWriteFile(output, (tdata_t) "/Range [0.0 1.0] \r", 18);
	buflen=sprintf(buffer, "/Size [%u] \r", (1<<t2p->tiff_bitspersample));
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) "/BitsPerSample 16 \r", 19);
	written += t2p_write_pdf_stream_dict(1<<(t2p->tiff_bitspersample+1), 0, output);

	return(written);
}

/*
	This function writes a palette stream for an indexed color space to output.
*/

tsize_t  CPdf::t2p_write_pdf_xobject_palettecs_stream(T2P* t2p, TIFF* output){

	tsize_t written=0;

	written += t2p_write_pdf_stream(
				(tdata_t) t2p->pdf_palette, 
				(tsize_t) t2p->pdf_palettesize, 
				output);
	
	return(written);
}
/*
	This function writes a PDF trailer to output.
*/

tsize_t  CPdf::t2p_write_pdf_trailer(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	char buffer[32];
	int buflen=0;
	char fileidbuf[16];
	int i=0;

	((int*)fileidbuf)[0] = rand();
	((int*)fileidbuf)[1] = rand();
	((int*)fileidbuf)[2] = rand();
	((int*)fileidbuf)[3] = rand();
	t2p->pdf_fileid=(char*)_TIFFmalloc(33);
	if(t2p->pdf_fileid==NULL)
	{
		t2p->t2p_error = T2P_ERR_ERROR;
		return(0);
	}
	_TIFFmemset(t2p->pdf_fileid, 0x00, 33);
	for (i=0;i<16;i++){
		sprintf(&(t2p->pdf_fileid[2*i]), "%.2X", fileidbuf[i]);
	}
	written += TIFFWriteFile(output, (tdata_t) "trailer\r<<\r/Size ", 17);
	buflen=sprintf(buffer, "%lu", t2p->pdf_xrefcount+1);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	_TIFFmemset(buffer, 0x00, 32);	
	written += TIFFWriteFile(output, (tdata_t) "\r/Root ", 7);
	buflen=sprintf(buffer, "%lu", t2p->pdf_catalog);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	_TIFFmemset(buffer, 0x00, 32);	
	written += TIFFWriteFile(output, (tdata_t) " 0 R \r/Info ", 12);
	buflen=sprintf(buffer, "%lu", t2p->pdf_info);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	_TIFFmemset(buffer, 0x00, 32);	
	written += TIFFWriteFile(output, (tdata_t) " 0 R \r/ID[<", 11);
	written += TIFFWriteFile(output, (tdata_t) t2p->pdf_fileid, 32);
	written += TIFFWriteFile(output, (tdata_t) "><", 2);
	written += TIFFWriteFile(output, (tdata_t) t2p->pdf_fileid, 32);
	written += TIFFWriteFile(output, (tdata_t) ">]\r>>\rstartxref\r", 16);
	buflen=sprintf(buffer, "%lu", t2p->pdf_startxref);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	_TIFFmemset(buffer, 0x00, 32);	
	written += TIFFWriteFile(output, (tdata_t) "\r%%EOF\r", 7);

	return(written);
}

/* 
	This function writes the PDF header to output.
*/

tsize_t CPdf::t2p_write_pdf_header(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	char buffer[16];
	int buflen=0;
	
	buflen=sprintf(buffer, "%%PDF-%u.%u ", t2p->pdf_majorversion&0xff, t2p->pdf_minorversion&0xff);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t)"\r%\342\343\317\323\r\n", 8);

	return(written);
}

/*
	This function sets the input directory to the directory of a given page and determines 
	information about the image.  
	
	It checks the image characteristics to determine if it is possible to convert the image 
	data into a page of PDF output, setting values of the T2P struct for this page.
	
	It determines what color space is used in the output PDF to represent the image.
	
	It determines if the image can be converted as raw data without requiring transcoding of 
	the image data.
	
*/

void CPdf::t2p_read_tiff_data(T2P* t2p, CIBitmap &m_ciBitmap)
{
	int i=0;
	uint16* r;
	uint16* g;
	uint16* b;
	uint16* a;
	uint16 xuint16;
	uint16* xuint16p;
	float* xfloatp;

	t2p->pdf_transcode = T2P_TRANSCODE_ENCODE;
	t2p->pdf_sample = T2P_SAMPLE_NOTHING;
    t2p->pdf_switchdecode = t2p->pdf_colorspace_invert;
        
	
	t2p->tiff_width = m_ciBitmap.GetBitmapWidth();
	t2p->tiff_length = m_ciBitmap.GetBitmapHeigth();
	t2p->tiff_compression = 0;
	t2p->tiff_bitspersample = 8;
	t2p->tiff_samplesperpixel = 4;

	t2p->pdf_colorspace=T2P_CS_RGB;
	t2p->pdf_sample=T2P_SAMPLE_RGBA_TO_RGB;


	t2p_compose_pdf_page(t2p);

	t2p->pdf_transcode = T2P_TRANSCODE_ENCODE;
	if(t2p->pdf_nopassthrough==0){
		(void)0;
	}

	if(t2p->pdf_transcode!=T2P_TRANSCODE_RAW){
		t2p->pdf_compression = t2p->pdf_defaultcompression;
	}

	if(t2p->pdf_sample & T2P_SAMPLE_REALIZE_PALETTE){
		t2p->tiff_samplesperpixel=3;
		t2p->tiff_photometric=PHOTOMETRIC_RGB;
	}

	return;
}

/*
	This function scans the input TIFF file for pages.  It attempts to determine which IFD's of 
	the TIFF file contain image document pages.  For each, it gathers some information that has 
	to do with the output of the PDF document as a whole.  
*/

void CPdf::t2p_read_tiff_init(T2P* t2p, CIBitmap &m_ciBitmap)
{
	tdir_t directorycount=0;
	tdir_t i=0;
	uint16 pagen=0;
	uint16 paged=0;
	uint16 xuint16=0;

	directorycount = 1;
	t2p->tiff_pages = (T2P_PAGE*) _TIFFmalloc(directorycount * sizeof(T2P_PAGE));
	if(t2p->tiff_pages==NULL)
	{
		t2p->t2p_error = T2P_ERR_ERROR;
		return;
	}
	_TIFFmemset( t2p->tiff_pages, 0x00, directorycount * sizeof(T2P_PAGE));
	t2p->tiff_tiles = (T2P_TILES*) _TIFFmalloc(directorycount * sizeof(T2P_TILES));
	if(t2p->tiff_pages==NULL)
	{
		return;
	}
	_TIFFmemset( t2p->tiff_tiles, 0x00, directorycount * sizeof(T2P_TILES));

	t2p->tiff_pages[t2p->tiff_pagecount].page_number=t2p->tiff_pagecount;
	t2p->tiff_pages[t2p->tiff_pagecount].page_directory=i;

	t2p->tiff_pagecount++;
	
	t2p->pdf_xrefcount += 5;
	t2p->tiff_tiles[i].tiles_tilecount=	t2p->tiff_pages[i].page_tilecount;

	return;
}

/*
	This function returns the necessary size of a data buffer to contain the raw or 
	uncompressed image data from the input TIFF for a page.
*/

void CPdf::t2p_read_tiff_size(T2P* t2p, CIBitmap &m_ciBitmap)
{
	t2p->tiff_datasize = m_ciBitmap.GetBitmapSize();
	return;
}

/*
	This function writes the beginning of a PDF object to output.
*/

tsize_t CPdf::t2p_write_pdf_obj_start(uint32 number, TIFF* output)
{

	tsize_t written=0;
	char buffer[16];
	int buflen=0;

	buflen=sprintf(buffer, "%lu", number);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen );
	written += TIFFWriteFile(output, (tdata_t) " 0 obj\r", 7);

	return(written);
}

/*
	This function writes the end of a PDF object to output.
*/

tsize_t CPdf::t2p_write_pdf_obj_end(TIFF* output)
{

	tsize_t written=0;

	written += TIFFWriteFile(output, (tdata_t) "endobj\r", 7);

	return(written);
}

/*
	This function writes a PDF name object to output.
*/

tsize_t CPdf::t2p_write_pdf_name(char* name, TIFF* output)
{

	tsize_t written=0;
	uint32 i=0;
	char buffer[4];
	uint16 nexchar=0;
	uint32 namelen=0;
	
	namelen=strlen(name);
	if (namelen>126) {
		namelen=126;
	}
	written += TIFFWriteFile(output, (tdata_t) "/", 1);
	for (i=0;i<namelen;i++){
		if ( ((unsigned char)name[i]) < 0x21){
			sprintf(buffer, "#%.2X", name[i]);
			written += TIFFWriteFile(output, (tdata_t) buffer, 3);
			nexchar=1;
		}
		if ( ((unsigned char)name[i]) > 0x7E){
			sprintf(buffer, "#%.2X", name[i]); 
			written += TIFFWriteFile(output, (tdata_t) buffer, 3);
			nexchar=1;
		}
		if (nexchar==0){
			switch (name[i]){
				case 0x23:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x25:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x28:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x29:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x2F:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x3C:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x3E:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x5B:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x5D:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x7B:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				case 0x7D:
					sprintf(buffer, "#%.2X", name[i]); 
					written += TIFFWriteFile(output, (tdata_t) buffer, 3);
					break;
				default:
					written += TIFFWriteFile(output, (tdata_t) &name[i], 1);
			}
		}
		nexchar=0;
	}
	written += TIFFWriteFile(output, (tdata_t) " ", 1);

	return(written);
}

/*
	This function writes a PDF string object to output.
*/
	
tsize_t CPdf::t2p_write_pdf_string(char* pdfstr, TIFF* output)
{

	tsize_t written=0;
	uint32 i=0;
	char buffer[5];
	uint32 len=0;
	
	len=strlen(pdfstr);
	written += TIFFWriteFile(output, (tdata_t) "(", 1);
	for (i=0;i<len;i++){
		if((pdfstr[i]&0x80) || (pdfstr[i]==127) || (pdfstr[i]<32)){
			sprintf(buffer, "\\%.3o", pdfstr[i]);
			written += TIFFWriteFile(output, (tdata_t) buffer, 4);
		} else {
			switch (pdfstr[i]){
				case 0x08:
					written += TIFFWriteFile(output, (tdata_t) "\\b", 2);
					break;
				case 0x09:
					written += TIFFWriteFile(output, (tdata_t) "\\t", 2);
					break;
				case 0x0A:
					written += TIFFWriteFile(output, (tdata_t) "\\n", 2);
					break;
				case 0x0C:
					written += TIFFWriteFile(output, (tdata_t) "\\f", 2);
					break;
				case 0x0D:
					written += TIFFWriteFile(output, (tdata_t) "\\r", 2);
					break;
				case 0x28:
					written += TIFFWriteFile(output, (tdata_t) "\\(", 2);
					break;
				case 0x29:
					written += TIFFWriteFile(output, (tdata_t) "\\)", 2);
					break;
				case 0x5C:
					written += TIFFWriteFile(output, (tdata_t) "\\\\", 2);
					break;
				default:
					written += TIFFWriteFile(output, (tdata_t) &pdfstr[i], 1);
			}
		}
	}
	written += TIFFWriteFile(output, (tdata_t) ") ", 1);

	return(written);
}

tsize_t CPdf::t2p_write_pdf_catalog(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	char buffer[16];
	int buflen=0;

	written += TIFFWriteFile(output, 
		(tdata_t)"<< \r/Type /Catalog \r/Pages ", 
		27);
	buflen=sprintf(buffer, "%lu", t2p->pdf_pages);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen );
	written += TIFFWriteFile(output, (tdata_t) " 0 R \r", 6);
	if(t2p->pdf_fitwindow){
		written += TIFFWriteFile(output, 
			(tdata_t) "/ViewerPreferences <</FitWindow true>>\r", 
			39);
	}
	written += TIFFWriteFile(output, (tdata_t)">>\r", 3);

	return(written);
}

tsize_t CPdf::t2p_sample_rgba_to_rgb(tdata_t data, uint32 samplecount)
{
	uint32 i = 0;
	uint32 sample = 0;
	uint8 alpha = 0;
	
	for (i = 0; i < samplecount; i++) 
	{
		sample=((uint32*)data)[i];
		//alpha=(uint8)((255 - (sample & 0xff)));
		//((uint8 *)data)[i * 3] = (uint8) ((sample >> 8) & 0xff) + alpha;
		//((uint8 *)data)[i * 3 + 1] = (uint8) ((sample >> 24) & 0xff) + alpha;
		//((uint8 *)data)[i * 3 + 2] = (uint8) ((sample >> 16) & 0xff) + alpha;
		((uint8 *)data)[i * 3] = (uint8) ((sample >> 16) & 0xff) + alpha;
		((uint8 *)data)[i * 3 + 1] = (uint8) ((sample >> 8) & 0xff) + alpha;
		((uint8 *)data)[i * 3 + 2] = (uint8) ((sample) & 0xff) + alpha;		
	}

	return (i * 3);
}

/*
	This function reads the raster image data from the input TIFF for an image and writes 
	the data to the output PDF XObject image dictionary stream.  It returns the amount written 
	or zero on error.
*/

tsize_t CPdf::t2p_readwrite_pdf_image(T2P* t2p, CIBitmap &m_ciBitmap, TIFF* output)
{

	tsize_t written=0;
	BYTE * buffer=NULL;
	unsigned char* samplebuffer=NULL;
	tsize_t bufferoffset=0;
	tsize_t samplebufferoffset=0;
	tsize_t read=0;
	tstrip_t i=0;
	tstrip_t j=0;
	tstrip_t stripcount=0;
	tsize_t stripsize=0;
	tsize_t sepstripcount=0;
	tsize_t sepstripsize=0;
	unsigned char* jpt;
	uint16 xuint16_1=0;
	uint16 xuint16_2=0;
	float* xfloatp;
	uint32* sbc;
	unsigned char* stripbuffer;
	uint32 striplength=0;
	uint32 max_striplength=0;

	t2p->pdf_compression = T2P_COMPRESS_NONE;


	switch(t2p->pdf_compression){
	case T2P_COMPRESS_NONE:
		TIFFSetField(output, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
		break;
	default:
		break;
	}

	//m_ciBitmap.HorzFlipBuf();

	buffer = (unsigned char*) _TIFFmalloc(t2p->tiff_datasize);
	if(buffer==NULL)
	{
		t2p->t2p_error = T2P_ERR_ERROR;
		return(0);
	}

	m_ciBitmap.VertFlipBuf();
	memcpy(buffer,m_ciBitmap.GetPtBitmap(),t2p->tiff_datasize);
	m_ciBitmap.VertFlipBuf();

	//m_ciBitmap.HorzFlipBuf();

	t2p->tiff_datasize=t2p_sample_rgba_to_rgb((tdata_t)buffer, t2p->tiff_width*t2p->tiff_length);

	TIFFSetField(output, TIFFTAG_PHOTOMETRIC, t2p->tiff_photometric);
	TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, t2p->tiff_bitspersample);
	TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, t2p->tiff_samplesperpixel);
	TIFFSetField(output, TIFFTAG_IMAGEWIDTH, t2p->tiff_width);
	TIFFSetField(output, TIFFTAG_IMAGELENGTH, t2p->tiff_length);
	TIFFSetField(output, TIFFTAG_ROWSPERSTRIP, t2p->tiff_length);
	TIFFSetField(output, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(output, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);

	switch(t2p->pdf_compression)
	{
	case T2P_COMPRESS_NONE:
		TIFFSetField(output, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
		break;
	default:
		break;
	}

	bufferoffset=TIFFWriteEncodedStrip(output, (tstrip_t)0, buffer,t2p->tiff_datasize); 
	if(buffer != NULL)
	{
		_TIFFfree(buffer);
		buffer=NULL;
	}

	if(bufferoffset==(tsize_t)-1)
	{
		t2p->t2p_error = T2P_ERR_ERROR;
		return(0);
	}
	
	written= output->tif_dir.td_stripbytecount[0];
	
	return(written);
}

/*
	This function writes the PDF Info structure to output.
*/

tsize_t CPdf::t2p_write_pdf_info(T2P* t2p, CIBitmap &m_ciBitmap, TIFF* output)
{

	tsize_t written=0;
	char* info;
	char buffer[512];
	int buflen=0;
	
	if(t2p->pdf_datetime==NULL)
	{
		t2p_pdf_tifftime(t2p);
	}

	if(strlen(t2p->pdf_datetime) > 0)
	{
		written += TIFFWriteFile(output, (tdata_t) "<< \r/CreationDate ", 18);
		written += t2p_write_pdf_string(t2p->pdf_datetime, output);
		written += TIFFWriteFile(output, (tdata_t) "\r/ModDate ", 10);
		written += t2p_write_pdf_string(t2p->pdf_datetime, output);
	}

	written += TIFFWriteFile(output, (tdata_t) "\r/Producer ", 11);
	_TIFFmemset((tdata_t)buffer, 0x00, 512);
	buflen=sprintf(buffer, "libtiff / tiff2pdf - 3.5.1 / %s", T2P_VERSION);
	written += t2p_write_pdf_string(buffer, output);
	written += TIFFWriteFile(output, (tdata_t) "\r", 1);

	if(t2p->pdf_creator != NULL)
	{ 
		if(strlen(t2p->pdf_creator)>0)
		{
			if(strlen(t2p->pdf_creator)>511)
			{
				t2p->pdf_creator[512]=(char)0;
			}
			written += TIFFWriteFile(output, (tdata_t) "/Creator ", 9);
			written += t2p_write_pdf_string(t2p->pdf_creator, output);
			written += TIFFWriteFile(output, (tdata_t) "\r", 1);
		}
	} 

	if(t2p->pdf_author != NULL)
	{ 
		if(strlen(t2p->pdf_author)>0)
		{
			if(strlen(t2p->pdf_author)>511)
			{
				t2p->pdf_author[512]=(char)0;
			}
			written += TIFFWriteFile(output, (tdata_t) "/Author ", 8);
			written += t2p_write_pdf_string(t2p->pdf_author, output);
			written += TIFFWriteFile(output, (tdata_t) "\r", 1);
		}
	} 

	if(t2p->pdf_title != NULL)
	{
		if(strlen(t2p->pdf_title)>0)
		{
			if(strlen(t2p->pdf_title)>511)
			{
				t2p->pdf_title[512]=(char)0;
			}
			written += TIFFWriteFile(output, (tdata_t) "/Title ", 7);
			written += t2p_write_pdf_string(t2p->pdf_title, output);
			written += TIFFWriteFile(output, (tdata_t) "\r", 1);
		}
	} 

	if(t2p->pdf_subject != NULL)
	{
		if(strlen(t2p->pdf_subject)>0)
		{
			if(strlen(t2p->pdf_subject)>511)
			{
				t2p->pdf_subject[512]=(char)0;
			}
			written += TIFFWriteFile(output, (tdata_t) "/Subject ", 9);
			written += t2p_write_pdf_string(t2p->pdf_subject, output);
			written += TIFFWriteFile(output, (tdata_t) "\r", 1);
		}
	} 

	if(t2p->pdf_keywords != NULL)
	{ 
		if(strlen(t2p->pdf_keywords)>0)
		{
			if(strlen(t2p->pdf_keywords)>511)
			{
				t2p->pdf_keywords[512]=(char)0;
			}
			written += TIFFWriteFile(output, (tdata_t) "/Keywords ", 10);
			written += t2p_write_pdf_string(t2p->pdf_keywords, output);
			written += TIFFWriteFile(output, (tdata_t) "\r", 1);
		}
	}
	written += TIFFWriteFile(output, (tdata_t) ">> \r", 4);	

	return(written);
}

/*
	This function fills a string of a T2P struct with the date and time of a TIFF file if it 
	exists or the current time as a PDF date string.
*/

void CPdf::t2p_pdf_tifftime(T2P* t2p)
{
	t2p->pdf_datetime= (char*) _TIFFmalloc(19);
	t2p->pdf_datetime[16]=0;
	t2p_pdf_currenttime(t2p);
	return;
}

/*
	This function fills a string of a T2P struct with the current time as a PDF date string, 
	it is called by t2p_pdf_tifftime.
*/

void CPdf::t2p_pdf_currenttime(T2P* t2p)
{

	struct tm* currenttime;
	time_t timenow;

	timenow=time(0);
	currenttime=localtime(&timenow);
	sprintf(t2p->pdf_datetime, "D:%.4d%.2d%.2d%.2d%.2d%.2d",
		(currenttime->tm_year+1900) % 65536, 
		(currenttime->tm_mon+1) % 256, 
		(currenttime->tm_mday) % 256, 
		(currenttime->tm_hour) % 256, 
		(currenttime->tm_min) % 256, 
		(currenttime->tm_sec) % 256);

	return;
}

/*
	This function writes a PDF Pages Tree structure to output.
*/

tsize_t CPdf::t2p_write_pdf_pages(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	tdir_t i=0;
	char buffer[16];
	int buflen=0;

	int page=0;
	written += TIFFWriteFile(output, 
		(tdata_t) "<< \r/Type /Pages \r/Kids [ ", 
		26);
	page = t2p->pdf_pages+1;
	for (i=0;i<t2p->tiff_pagecount;i++){
		buflen=sprintf(buffer, "%d", page);
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " 0 R ", 5);
		if ( ((i+1)%8)==0 ) {
			written += TIFFWriteFile(output, (tdata_t) "\r", 1);
		}
		page +=3;
		page += t2p->tiff_pages[i].page_extra;
		if(t2p->tiff_pages[i].page_tilecount>0){
			page += (2 * t2p->tiff_pages[i].page_tilecount);
		} else {
			page +=2;
		}
	}
	written += TIFFWriteFile(output, (tdata_t) "] \r/Count ", 10);
	_TIFFmemset(buffer, 0x00, 16);
	buflen=sprintf(buffer, "%d", t2p->tiff_pagecount);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) " \r>> \r", 6);

	return(written);
}

/*
	This function writes a PDF Page structure to output.
*/

tsize_t CPdf::t2p_write_pdf_page(uint32 object, T2P* t2p, TIFF* output)
{

	int i=0;
	tsize_t written=0;
	char buffer[16];
	int buflen=0;
	
	written += TIFFWriteFile(output, (tdata_t) "<<\r/Type /Page \r/Parent ", 24);
	buflen=sprintf(buffer, "%lu", t2p->pdf_pages);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) " 0 R \r", 6);
	written += TIFFWriteFile(output, (tdata_t) "/MediaBox [", 11); 
	buflen=sprintf(buffer, "%.4f",t2p->pdf_mediabox.x1);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) " ", 1); 
	buflen=sprintf(buffer, "%.4f",t2p->pdf_mediabox.y1);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) " ", 1); 
	buflen=sprintf(buffer, "%.4f",t2p->pdf_mediabox.x2);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) " ", 1); 
	buflen=sprintf(buffer, "%.4f",t2p->pdf_mediabox.y2);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) "] \r", 3); 
	written += TIFFWriteFile(output, (tdata_t) "/Contents ", 10);
	buflen=sprintf(buffer, "%lu", object+1);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) " 0 R \r", 6);
	written += TIFFWriteFile(output, (tdata_t) "/Resources << \r", 15);
	if( t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount != 0 ){
		written += TIFFWriteFile(output, (tdata_t) "/XObject <<\r", 12);
		for(i=0;i<t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount;i++){
			written += TIFFWriteFile(output, (tdata_t) "/Im", 3);
			buflen = sprintf(buffer, "%u", t2p->pdf_page+1);
			written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
			written += TIFFWriteFile(output, (tdata_t) "_", 1);
			buflen = sprintf(buffer, "%u", i+1);
			written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
			written += TIFFWriteFile(output, (tdata_t) " ", 1);
			buflen = sprintf(
				buffer, 
				"%lu", 
				object+3+(2*i)+t2p->tiff_pages[t2p->pdf_page].page_extra); 
			written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
			written += TIFFWriteFile(output, (tdata_t) " 0 R ", 5);
			if(i%4==3){
				written += TIFFWriteFile(output, (tdata_t) "\r", 1);
			}
		}
		written += TIFFWriteFile(output, (tdata_t) ">>\r", 3);
	} else {
			written += TIFFWriteFile(output, (tdata_t) "/XObject <<\r", 12);
			written += TIFFWriteFile(output, (tdata_t) "/Im", 3);
			buflen = sprintf(buffer, "%u", t2p->pdf_page+1);
			written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
			written += TIFFWriteFile(output, (tdata_t) " ", 1);
			buflen = sprintf(
				buffer, 
				"%lu", 
				object+3+(2*i)+t2p->tiff_pages[t2p->pdf_page].page_extra); 
			written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
			written += TIFFWriteFile(output, (tdata_t) " 0 R ", 5);
		written += TIFFWriteFile(output, (tdata_t) ">>\r", 3);
	}

	written += TIFFWriteFile(output, (tdata_t) "/ProcSet [ ", 11);
	if(t2p->pdf_colorspace == T2P_CS_BILEVEL 
		|| t2p->pdf_colorspace == T2P_CS_GRAY
		){
		written += TIFFWriteFile(output, (tdata_t) "/ImageB ", 8);
	} else {
		written += TIFFWriteFile(output, (tdata_t) "/ImageC ", 8);
		if(t2p->pdf_colorspace & T2P_CS_PALETTE){
			written += TIFFWriteFile(output, (tdata_t) "/ImageI ", 8);
		}
	}
	written += TIFFWriteFile(output, (tdata_t) "]\r>>\r>>\r", 8);

	return(written);
}

/*
	This functions writes the beginning of a PDF stream dictionary to output.
*/

tsize_t CPdf::t2p_write_pdf_stream_dict_start(TIFF* output)
{

	tsize_t written=0;

	written += TIFFWriteFile(output, (tdata_t) "<< \r", 4);

	return(written);
}

/*
	This function writes a stream dictionary for a PDF stream to output.
*/

tsize_t CPdf::t2p_write_pdf_stream_dict(tsize_t len, uint32 number, TIFF* output)
{
	
	tsize_t written=0;
	char buffer[16];
	int buflen=0;
	
	written += TIFFWriteFile(output, (tdata_t) "/Length ", 8);
	if(len!=0){
		written += t2p_write_pdf_stream_length(len, output);
	} else {
		buflen=sprintf(buffer, "%lu", number);
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " 0 R \r", 6);
	}
	
	return(written);
}

/*
	This function writes the end of a PDF stream dictionary to output. 
*/

tsize_t CPdf::t2p_write_pdf_stream_dict_end(TIFF* output)
{

	tsize_t written=0;

	written += TIFFWriteFile(output, (tdata_t) " >>\r", 4);

	return(written);
}

/*
	This functions writes the beginning of a PDF stream to output.
*/

tsize_t CPdf::t2p_write_pdf_stream_start(TIFF* output)
{

	tsize_t written=0;

	written += TIFFWriteFile(output, (tdata_t) "stream\r\n", 8);

	return(written);
}

/*
	This function writes a PDF Contents stream to output.
*/

tsize_t CPdf::t2p_write_pdf_page_content_stream(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	ttile_t i=0;
	char buffer[512];
	int buflen=0;
	T2P_BOX box;
	
	box=t2p->pdf_imagebox;
	buflen=sprintf(buffer, 
		"q %s %.4f %.4f %.4f %.4f %.4f %.4f cm /Im%d Do Q\r", 
		t2p->tiff_transferfunctioncount?"/GS1 gs ":"",
		box.mat[0],
		box.mat[1],
		box.mat[3],
		box.mat[4],
		box.mat[6],
		box.mat[7],
		t2p->pdf_page+1);
	written += t2p_write_pdf_stream(buffer, buflen, output);


	return(written);
}

/*
	This function writes the end of a PDF stream to output. 
*/

tsize_t CPdf::t2p_write_pdf_stream_end(TIFF* output)
{

	tsize_t written=0;

	written += TIFFWriteFile(output, (tdata_t) "\rendstream\r", 11);

	return(written);
}

/*
	This function writes a number to output.
*/

tsize_t CPdf::t2p_write_pdf_stream_length(tsize_t len, TIFF* output)
{

	tsize_t written=0;
	char buffer[16];
	int buflen=0;

	buflen=sprintf(buffer, "%lu", len);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) "\r", 1);

	return(written);
}

/*
	This function writes a PDF Image XObject stream dictionary to output. 
*/

tsize_t CPdf::t2p_write_pdf_xobject_stream_dict(ttile_t tile, 
												T2P* t2p, 
												TIFF* output){

	tsize_t written=0;
	char buffer[16];
	int buflen=0;

	written += t2p_write_pdf_stream_dict(0, t2p->pdf_xrefcount+1, output); 
	written += TIFFWriteFile(output, 
		(tdata_t) "/Type /XObject \r/Subtype /Image \r/Name /Im", 
		42);
	buflen=sprintf(buffer, "%u", t2p->pdf_page+1);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	if(tile != 0){
		written += TIFFWriteFile(output, (tdata_t) "_", 1);
		buflen=sprintf(buffer, "%lu", tile);
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	}
	written += TIFFWriteFile(output, (tdata_t) "\r/Width ", 8);
	_TIFFmemset((tdata_t)buffer, 0x00, 16);
	if(tile==0){
		buflen=sprintf(buffer, "%lu", t2p->tiff_width);
	} else {
		if(t2p_tile_is_right_edge(t2p->tiff_tiles[t2p->pdf_page], tile-1)!=0){
			buflen=sprintf(
				buffer, 
				"%lu", 
				t2p->tiff_tiles[t2p->pdf_page].tiles_edgetilewidth);
		} else {
			buflen=sprintf(
				buffer, 
				"%lu", 
				t2p->tiff_tiles[t2p->pdf_page].tiles_tilewidth);
		}
	}
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) "\r/Height ", 9);
	_TIFFmemset((tdata_t)buffer, 0x00, 16);
	if(tile==0){
		buflen=sprintf(buffer, "%lu", t2p->tiff_length);
	} else {
		if(t2p_tile_is_bottom_edge(t2p->tiff_tiles[t2p->pdf_page], tile-1)!=0){
			buflen=sprintf(
				buffer, 
				"%lu", 
				t2p->tiff_tiles[t2p->pdf_page].tiles_edgetilelength);
		} else {
			buflen=sprintf(
				buffer, 
				"%lu", 
				t2p->tiff_tiles[t2p->pdf_page].tiles_tilelength);
		}
	}
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) "\r/BitsPerComponent ", 19);
	_TIFFmemset((tdata_t)buffer, 0x00, 16);
	buflen=sprintf(buffer, "%u", t2p->tiff_bitspersample);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) "\r/ColorSpace ", 13);
	written += t2p_write_pdf_xobject_cs(t2p, output);
	if( (t2p->pdf_switchdecode != 0))
	{
		written += t2p_write_pdf_xobject_decode(t2p, output);
	}
	written += t2p_write_pdf_xobject_stream_filter(tile, t2p, output);

	return(written);
}

												
/*
	This function calls TIFFWriteDirectory on the output after blanking its output by replacing the 
	read, write, and seek procedures with empty implementations, then it replaces the original 
	implementations.
*/

void CPdf::t2p_write_advance_directory(T2P* t2p, TIFF* output)
{

	t2p->tiff_writeproc=output->tif_writeproc;
	output->tif_writeproc=t2p_empty_writeproc;
	t2p->tiff_readproc=output->tif_readproc;
	output->tif_readproc=t2p_empty_readproc;
	t2p->tiff_seekproc=output->tif_seekproc;
	output->tif_seekproc=t2p_empty_seekproc;
	output->tif_header.tiff_diroff=0;
	if(!TIFFWriteDirectory(output)){
		TIFFError(TIFF2PDF_MODULE, 
			"Error writing virtual directory to output PDF %s", 
			TIFFFileName(output));
		t2p->t2p_error = T2P_ERR_ERROR;
		return;
	}
	output->tif_writeproc=t2p->tiff_writeproc;
	output->tif_readproc=t2p->tiff_readproc;
	output->tif_seekproc=t2p->tiff_seekproc;
	
	return;
}

/*
	This function writes a PDF xref table to output.
*/

tsize_t CPdf::t2p_write_pdf_xreftable(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	char buffer[21];
	int buflen=0;
	uint32 i=0;

	written += TIFFWriteFile(output, (tdata_t) "xref\r0 ", 7);
	buflen=sprintf(buffer, "%lu", t2p->pdf_xrefcount+1);
	written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
	written += TIFFWriteFile(output, (tdata_t) " \r0000000000 65535 f\r\n", 22);
	for (i=0;i<t2p->pdf_xrefcount;i++){
		sprintf(buffer, "%.10lu 00000 n\r\n", t2p->pdf_xrefoffsets[i]);
		written += TIFFWriteFile(output, (tdata_t) buffer, 20);
	}

	return(written);
}


/*
	This function composes the page size and image and tile locations on a page.
*/

void CPdf::t2p_compose_pdf_page(T2P* t2p)
{

	uint32 i=0;
	uint32 i2=0;
	T2P_TILE* tiles=NULL;
	T2P_BOX* boxp=NULL;
	uint32 tilecountx=0;
	uint32 tilecounty=0;
	uint32 tilewidth=0;
	uint32 tilelength=0;
	int istiled=0;
	float f=0;
	
	t2p->pdf_xres = t2p->tiff_xres;
	t2p->pdf_yres = t2p->tiff_yres;
	if(t2p->pdf_overrideres){
		t2p->pdf_xres = t2p->pdf_defaultxres;
		t2p->pdf_yres = t2p->pdf_defaultyres;
	}
	if(t2p->pdf_xres==0.0){
		t2p->pdf_xres = t2p->pdf_defaultxres;
	}
	if(t2p->pdf_yres==0.0){
		t2p->pdf_yres = t2p->pdf_defaultyres;
	}
	t2p->pdf_imagewidth=((float)(t2p->tiff_width)) *72.0 / t2p->pdf_xres;
	t2p->pdf_imagelength=((float)(t2p->tiff_length)) *72.0 / t2p->pdf_yres;
	if(t2p->pdf_overridepagesize != 0){
		t2p->pdf_pagewidth = t2p->pdf_defaultpagewidth;
		t2p->pdf_pagelength = t2p->pdf_defaultpagelength;
	} else {
		t2p->pdf_pagewidth = t2p->pdf_imagewidth;
		t2p->pdf_pagelength = t2p->pdf_imagelength;
	}
	t2p->pdf_mediabox.x1=0.0;
	t2p->pdf_mediabox.y1=0.0;
	t2p->pdf_mediabox.x2=t2p->pdf_pagewidth;
	t2p->pdf_mediabox.y2=t2p->pdf_pagelength;
	t2p->pdf_imagebox.x1=0.0;
	t2p->pdf_imagebox.y1=0.0;
	t2p->pdf_imagebox.x2=t2p->pdf_imagewidth;
	t2p->pdf_imagebox.y2=t2p->pdf_imagelength;
	if(t2p->pdf_overridepagesize!=0){
		t2p->pdf_imagebox.x1+=((t2p->pdf_pagewidth-t2p->pdf_imagewidth)/2.0);
		t2p->pdf_imagebox.y1+=((t2p->pdf_pagelength-t2p->pdf_imagelength)/2.0);
		t2p->pdf_imagebox.x2+=((t2p->pdf_pagewidth-t2p->pdf_imagewidth)/2.0);
		t2p->pdf_imagebox.y2+=((t2p->pdf_pagelength-t2p->pdf_imagelength)/2.0);
	}
	if(t2p->tiff_orientation > 4){
		f=t2p->pdf_mediabox.x2;
		t2p->pdf_mediabox.x2=t2p->pdf_mediabox.y2;
		t2p->pdf_mediabox.y2=f;
	}
	istiled=((t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecount==0) ? 0 : 1;
	if(istiled==0){
		t2p_compose_pdf_page_orient(&(t2p->pdf_imagebox), t2p->tiff_orientation);
		return;
	} else {
		tilewidth=(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilewidth;
		tilelength=(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilelength;
		tilecountx=(t2p->tiff_width + 
			tilewidth -1)/ 
			tilewidth;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecountx=tilecountx;
		tilecounty=(t2p->tiff_length + 
			tilelength -1)/ 
			tilelength;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecounty=tilecounty;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_edgetilewidth=
			t2p->tiff_width % tilewidth;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_edgetilelength=
			t2p->tiff_length % tilelength;
		tiles=(t2p->tiff_tiles[t2p->pdf_page]).tiles_tiles;
		for(i2=0;i2<tilecounty-1;i2++){
			for(i=0;i<tilecountx-1;i++){
				boxp=&(tiles[i2*tilecountx+i].tile_box);
				boxp->x1 = 
					t2p->pdf_imagebox.x1 
					+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
					/ (float)t2p->tiff_width);
				boxp->x2 = 
					t2p->pdf_imagebox.x1 
					+ ((float)(t2p->pdf_imagewidth * (i+1) * tilewidth)
					/ (float)t2p->tiff_width);
				boxp->y1 = 
					t2p->pdf_imagebox.y2 
					- ((float)(t2p->pdf_imagelength * (i2+1) * tilelength)
					/ (float)t2p->tiff_length);
				boxp->y2 = 
					t2p->pdf_imagebox.y2 
					- ((float)(t2p->pdf_imagelength * i2 * tilelength)
					/ (float)t2p->tiff_length);
			}
			boxp=&(tiles[i2*tilecountx+i].tile_box);
			boxp->x1 = 
				t2p->pdf_imagebox.x1 
				+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
				/ (float)t2p->tiff_width);
			boxp->x2 = t2p->pdf_imagebox.x2;
			boxp->y1 = 
				t2p->pdf_imagebox.y2 
				- ((float)(t2p->pdf_imagelength * (i2+1) * tilelength)
				/ (float)t2p->tiff_length);
			boxp->y2 = 
				t2p->pdf_imagebox.y2 
				- ((float)(t2p->pdf_imagelength * i2 * tilelength)
				/ (float)t2p->tiff_length);
		}
		for(i=0;i<tilecountx-1;i++){
			boxp=&(tiles[i2*tilecountx+i].tile_box);
			boxp->x1 = 
				t2p->pdf_imagebox.x1 
				+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
				/ (float)t2p->tiff_width);
			boxp->x2 = 
				t2p->pdf_imagebox.x1 
				+ ((float)(t2p->pdf_imagewidth * (i+1) * tilewidth)
				/ (float)t2p->tiff_width);
			boxp->y1 = t2p->pdf_imagebox.y1;
			boxp->y2 = 
				t2p->pdf_imagebox.y2 
				- ((float)(t2p->pdf_imagelength * i2 * tilelength)
				/ (float)t2p->tiff_length);
		}
		boxp=&(tiles[i2*tilecountx+i].tile_box);
		boxp->x1 = 
			t2p->pdf_imagebox.x1 
			+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
			/ (float)t2p->tiff_width);
		boxp->x2 = t2p->pdf_imagebox.x2;
		boxp->y1 = t2p->pdf_imagebox.y1;
		boxp->y2 = 
			t2p->pdf_imagebox.y2 
			- ((float)(t2p->pdf_imagelength * i2 * tilelength)
			/ (float)t2p->tiff_length);
	}
	if(t2p->tiff_orientation==0 || t2p->tiff_orientation==1){
		for(i=0;i<(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecount;i++){
			t2p_compose_pdf_page_orient( &(tiles[i].tile_box) , 0);
		}
		return;
	}
	for(i=0;i<(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecount;i++){
		boxp=&(tiles[i].tile_box);
		boxp->x1 -= t2p->pdf_imagebox.x1;
		boxp->x2 -= t2p->pdf_imagebox.x1;
		boxp->y1 -= t2p->pdf_imagebox.y1;
		boxp->y2 -= t2p->pdf_imagebox.y1;
		if(t2p->tiff_orientation==2 || t2p->tiff_orientation==3){
			boxp->x1 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x1;
			boxp->x2 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x2;
		}
		if(t2p->tiff_orientation==3 || t2p->tiff_orientation==4){
			boxp->y1 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y1;
			boxp->y2 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y2;
		}
		if(t2p->tiff_orientation==8 || t2p->tiff_orientation==5){
			boxp->y1 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y1;
			boxp->y2 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y2;
		}
		if(t2p->tiff_orientation==5 || t2p->tiff_orientation==6){
			boxp->x1 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x1;
			boxp->x2 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x2;
		}
		if(t2p->tiff_orientation > 4){
			f=boxp->x1;
			boxp->x1 = boxp->y1;
			boxp->y1 = f;
			f=boxp->x2;
			boxp->x2 = boxp->y2;
			boxp->y2 = f; 
			t2p_compose_pdf_page_orient_flip(boxp, t2p->tiff_orientation);
		} else {
			t2p_compose_pdf_page_orient(boxp, t2p->tiff_orientation);
		}
		
	}

	return;
}

/*
	This function writes a buffer of data to output.
*/

tsize_t CPdf::t2p_write_pdf_stream(tdata_t buffer, tsize_t len, TIFF* output)
{

	tsize_t written=0;

	written += TIFFWriteFile(output, (tdata_t) buffer, len);

	return(written);
}


void  CPdf::t2p_compose_pdf_page_orient(T2P_BOX* boxp, uint16 orientation)
{

	float m1[9];
	float f=0.0;
	
	if( boxp->x1 > boxp->x2){
		f=boxp->x1;
		boxp->x1=boxp->x2;
		boxp->x2 = f;
	}
	if( boxp->y1 > boxp->y2){
		f=boxp->y1;
		boxp->y1=boxp->y2;
		boxp->y2 = f;
	}
	boxp->mat[0]=m1[0]=boxp->x2-boxp->x1;
	boxp->mat[1]=m1[1]=0.0;
	boxp->mat[2]=m1[2]=0.0;
	boxp->mat[3]=m1[3]=0.0;
	boxp->mat[4]=m1[4]=boxp->y2-boxp->y1;
	boxp->mat[5]=m1[5]=0.0;
	boxp->mat[6]=m1[6]=boxp->x1;
	boxp->mat[7]=m1[7]=boxp->y1;
	boxp->mat[8]=m1[8]=1.0;
	switch(orientation){
		case 0:
		case 1:
			break;
		case 2:
			boxp->mat[0]=0.0-m1[0];
			boxp->mat[6]+=m1[0];
			break;
		case 3:
			boxp->mat[0]=0.0-m1[0];
			boxp->mat[4]=0.0-m1[4];
			boxp->mat[6]+=m1[0];
			boxp->mat[7]+=m1[4];
			break;
		case 4:
			boxp->mat[4]=0.0-m1[4];
			boxp->mat[7]+=m1[4];
			break;
		case 5:
			boxp->mat[0]=0.0;
			boxp->mat[1]=0.0-m1[0];
			boxp->mat[3]=0.0-m1[4];
			boxp->mat[4]=0.0;
			boxp->mat[6]+=m1[4];
			boxp->mat[7]+=m1[0];
			break;
		case 6:
			boxp->mat[0]=0.0;
			boxp->mat[1]=0.0-m1[0];
			boxp->mat[3]=m1[4];
			boxp->mat[4]=0.0;
			boxp->mat[7]+=m1[0];
			break;
		case 7:
			boxp->mat[0]=0.0;
			boxp->mat[1]=m1[0];
			boxp->mat[3]=m1[4];
			boxp->mat[4]=0.0;
			break;
		case 8:
			boxp->mat[0]=0.0;
			boxp->mat[1]=m1[0];
			boxp->mat[3]=0.0-m1[4];
			boxp->mat[4]=0.0;
			boxp->mat[6]+=m1[4];
			break;
	}

	return;
}


void CPdf::t2p_compose_pdf_page_orient_flip(T2P_BOX* boxp, uint16 orientation){

	float m1[9];
	float f=0.0;
	
	if( boxp->x1 > boxp->x2){
		f=boxp->x1;
		boxp->x1=boxp->x2;
		boxp->x2 = f;
	}
	if( boxp->y1 > boxp->y2){
		f=boxp->y1;
		boxp->y1=boxp->y2;
		boxp->y2 = f;
	}
	boxp->mat[0]=m1[0]=boxp->x2-boxp->x1;
	boxp->mat[1]=m1[1]=0.0;
	boxp->mat[2]=m1[2]=0.0;
	boxp->mat[3]=m1[3]=0.0;
	boxp->mat[4]=m1[4]=boxp->y2-boxp->y1;
	boxp->mat[5]=m1[5]=0.0;
	boxp->mat[6]=m1[6]=boxp->x1;
	boxp->mat[7]=m1[7]=boxp->y1;
	boxp->mat[8]=m1[8]=1.0;
	switch(orientation){
		case 5:
			boxp->mat[0]=0.0;
			boxp->mat[1]=0.0-m1[4];
			boxp->mat[3]=0.0-m1[0];
			boxp->mat[4]=0.0;
			boxp->mat[6]+=m1[0];
			boxp->mat[7]+=m1[4];
			break;
		case 6:
			boxp->mat[0]=0.0;
			boxp->mat[1]=0.0-m1[4];
			boxp->mat[3]=m1[0];
			boxp->mat[4]=0.0;
			boxp->mat[7]+=m1[4];
			break;
		case 7:
			boxp->mat[0]=0.0;
			boxp->mat[1]=m1[4];
			boxp->mat[3]=m1[0];
			boxp->mat[4]=0.0;
			break;
		case 8:
			boxp->mat[0]=0.0;
			boxp->mat[1]=m1[4];
			boxp->mat[3]=0.0-m1[0];
			boxp->mat[4]=0.0;
			boxp->mat[6]+=m1[0];
			break;
	}

	return;
}

/*
	This functions returns a non-zero value when the tile is on the right edge 
	and does not have full imaged tile width.
*/

int CPdf::t2p_tile_is_right_edge(T2P_TILES tiles, ttile_t tile){

	if( ((tile+1) % tiles.tiles_tilecountx == 0) 
		&& (tiles.tiles_edgetilewidth != 0) ){
		return(1);
	} else {
		return(0);
	}
	
	return(0);
	
}
/*
	This functions returns a non-zero value when the tile is on the bottom edge 
	and does not have full imaged tile length.
*/

int CPdf::t2p_tile_is_bottom_edge(T2P_TILES tiles, ttile_t tile){

	if( ((tile+1) > (tiles.tiles_tilecount-tiles.tiles_tilecountx) )
		&& (tiles.tiles_edgetilelength != 0) ){
		return(1);
	} else {
		return(0);
	}
	
	return(0);
}


/*
	This function writes a PDF Image XObject Colorspace name to output.
*/

tsize_t CPdf::t2p_write_pdf_xobject_cs(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	char buffer[128];
	int buflen=0;

	float X_W=1.0;
	float Y_W=1.0;
	float Z_W=1.0;
	
	if( (t2p->pdf_colorspace & T2P_CS_PALETTE) != 0)
	{
		written += TIFFWriteFile(output, (tdata_t) "[ /Indexed ", 11);
		//t2p->pdf_colorspace = t2p->pdf_colorspace ^ T2P_CS_PALETTE;
		written += t2p_write_pdf_xobject_cs(t2p, output);
		//t2p->pdf_colorspace = t2p->pdf_colorspace | T2P_CS_PALETTE;
		buflen=sprintf(buffer, "%u", (0x0001 << t2p->tiff_bitspersample)-1 );
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " ", 1);
		_TIFFmemset(buffer, 0x00, 16);
		buflen=sprintf(buffer, "%lu", t2p->pdf_palettecs ); 
		written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
		written += TIFFWriteFile(output, (tdata_t) " 0 R ]\r", 7);
		return(written);
	}
	if(t2p->pdf_colorspace & T2P_CS_BILEVEL){
			written += TIFFWriteFile(output, (tdata_t) "/DeviceGray \r", 13);
	}
	if(t2p->pdf_colorspace & T2P_CS_GRAY){
				written += TIFFWriteFile(output, (tdata_t) "/DeviceGray \r", 13);
	}
	if(t2p->pdf_colorspace & T2P_CS_RGB){
				written += TIFFWriteFile(output, (tdata_t) "/DeviceRGB \r", 12);
	}
	if(t2p->pdf_colorspace & T2P_CS_LAB){
			written += TIFFWriteFile(output, (tdata_t) "[/Lab << \r", 10);
			written += TIFFWriteFile(output, (tdata_t) "/WhitePoint ", 12);
			written += TIFFWriteFile(output, (tdata_t) "/Range ", 7);
			buflen=sprintf(buffer, "[%d %d %d %d] \r", 
				t2p->pdf_labrange[0], 
				t2p->pdf_labrange[1], 
				t2p->pdf_labrange[2], 
				t2p->pdf_labrange[3]);
			written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
			written += TIFFWriteFile(output, (tdata_t) ">>] \r", 5);
			
	}
	
	return(written);
}

/*
	This function writes a PDF Image XObject Decode array to output.
*/

tsize_t CPdf::t2p_write_pdf_xobject_decode(T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	int i=0;

	written += TIFFWriteFile(output, (tdata_t) "/Decode [ ", 10);
	for (i=0;i<t2p->tiff_samplesperpixel;i++){
		written += TIFFWriteFile(output, (tdata_t) "1 0 ", 4);
	}
	written += TIFFWriteFile(output, (tdata_t) "]\r", 2);

	return(written);
}


/*
	This function writes a PDF Image XObject stream filter name and parameters to 
	output.
*/

tsize_t CPdf::t2p_write_pdf_xobject_stream_filter(ttile_t tile, T2P* t2p, TIFF* output)
{

	tsize_t written=0;
	char buffer[16];
	int buflen=0;

	if(t2p->pdf_compression==T2P_COMPRESS_NONE){
		return(written);
	}
	written += TIFFWriteFile(output, (tdata_t) "/Filter ", 8);
	switch(t2p->pdf_compression)
	{

		case T2P_COMPRESS_JPEG:
			written += TIFFWriteFile(output, (tdata_t) "/DCTDecode ", 11);
			break;

		case T2P_COMPRESS_ZIP:
			written += TIFFWriteFile(output, (tdata_t) "/FlateDecode ", 13);
			if(t2p->pdf_compressionquality%100){
				written += TIFFWriteFile(output, (tdata_t) "/DecodeParms ", 13);
				written += TIFFWriteFile(output, (tdata_t) "<< /Predictor ", 14);
				_TIFFmemset(buffer, 0x00, 16);
				buflen=sprintf(buffer, "%u", t2p->pdf_compressionquality%100);
				written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
				written += TIFFWriteFile(output, (tdata_t) " /Columns ", 10);
				_TIFFmemset(buffer, 0x00, 16);
				buflen=sprintf(buffer, "%lu", t2p->tiff_width);
				written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
				written += TIFFWriteFile(output, (tdata_t) " /Colors ", 9);
				_TIFFmemset(buffer, 0x00, 16);
				buflen=sprintf(buffer, "%u", t2p->tiff_samplesperpixel);
				written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
				written += TIFFWriteFile(output, (tdata_t) " /BitsPerComponent ", 19);
				_TIFFmemset(buffer, 0x00, 16);
				buflen=sprintf(buffer, "%u", t2p->tiff_bitspersample);
				written += TIFFWriteFile(output, (tdata_t) buffer, buflen);
				written += TIFFWriteFile(output, (tdata_t) ">>\r", 3);
			}
			break;

		default:
			break;
	}

	return(written);
}