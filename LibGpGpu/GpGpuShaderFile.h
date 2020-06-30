static const char * BlurShaderSource = 
"float4 Blur(float2 coords : TEXCOORD0,									\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;							\n"       
"    static const float hOffset = 1.0;							\n"    
"    float4 c  = texRECT(texture, coords);                              \n"
"    float4 bl = texRECT(texture, coords + float2(-wOffset, -hOffset)); \n"
"    float4 l  = texRECT(texture, coords + float2(-wOffset,       0));  \n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 t  = texRECT(texture, coords + float2(      0,  hOffset));  \n"
"    float4 ur = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 r  = texRECT(texture, coords + float2( wOffset,       0));  \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 b  = texRECT(texture, coords + float2(      0, -hOffset));  \n"
"    return (c + bl + l + tl + t + ur + r + br + b) / 9.0;				\n"
"}																		\n";

static const char * SoftenShaderSource = 
"float4 Soften(float2 coords : TEXCOORD0,								\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;									\n"       
"    static const float hOffset = 1.0;									\n"    
"    float4 c  = texRECT(texture, coords);                              \n"
"    float4 bl = texRECT(texture, coords + float2(-wOffset, -hOffset)); \n"
"    float4 l  = texRECT(texture, coords + float2(-wOffset,       0));  \n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 t  = texRECT(texture, coords + float2(      0,  hOffset));  \n"
"    float4 ur = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 r  = texRECT(texture, coords + float2( wOffset,       0));  \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 b  = texRECT(texture, coords + float2(      0, -hOffset));  \n"
"    return (8 * c + bl + l + tl + t + ur + r + br + b) / 16.0;		    \n"
"}																		\n";

static const char * EdgeShaderSource = 
"float4 Edge(float2 coords : TEXCOORD0,									\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;									\n"
"    static const float hOffset = 1.0;									\n"
"    float4 c  = texRECT(texture, coords);                              \n"
"    float4 bl = texRECT(texture, coords + float2(-wOffset, -hOffset)); \n"
"    float4 l  = texRECT(texture, coords + float2(-wOffset,       0));  \n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 t  = texRECT(texture, coords + float2(      0,  hOffset));  \n"
"    float4 ur = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 r  = texRECT(texture, coords + float2( wOffset,       0));  \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 b  = texRECT(texture, coords + float2(      0, -hOffset));  \n"
"    // scale by 8 to brighten the edges								\n"
"    return 8 * (c + -0.125 * (bl + l + tl + t + ur + r + br + b));		\n"
"}																		\n";

/*----------------------------------------------------------------------*/
// This shader performs a 9-tap Laplacian edge detection filter.
//
//	This is the actual cg code that will be compiled at runtime
//	and loaded into the graphic card, fragment processor
//
//	tex2D returns the value of the texture at coordinates
//
//	the laplacian filter implemented is a 3x3 filter with the 
//	following coefficients:
//
//		-1	0	0		tl	t	tr
//		0	0	0		l	c	r
//		0	0	-1		bl	b	br
//
//
static const char * EmbossShaderSource = 
"float4 Emboss(float2 coords : TEXCOORD0,									\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;									\n"
"    static const float hOffset = 1.0;									\n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    return 127 - ((tl + br) / 9.0);									\n"
"}																		\n";

static const char * SharpenShaderSource = 
"float4 Sharpen(float2 coords : TEXCOORD0,								\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;									\n"
"    static const float hOffset = 1.0;									\n"
"    float4 c  = texRECT(texture, coords);                              \n"
"    float4 bl = texRECT(texture, coords + float2(-wOffset, -hOffset)); \n"
"    float4 l  = texRECT(texture, coords + float2(-wOffset,       0));  \n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 t  = texRECT(texture, coords + float2(      0,  hOffset));  \n"
"    float4 ur = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 r  = texRECT(texture, coords + float2( wOffset,       0));  \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 b  = texRECT(texture, coords + float2(      0, -hOffset));  \n"
"    // scale by 8 to brighten the edges								\n"
"    return (9 * c - (bl + l + tl + t + ur + r + br + b));				\n"
"}																		\n";

static const char * SharpenStrongShaderSource = 
"float4 SharpenStrong(float2 coords : TEXCOORD0,						\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;									\n"
"    static const float hOffset = 1.0;									\n"
"    float4 c  = texRECT(texture, coords);                              \n"
"    float4 bl = texRECT(texture, coords + float2(-wOffset, -hOffset)); \n"
"    float4 l  = texRECT(texture, coords + float2(-wOffset,       0));  \n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 t  = texRECT(texture, coords + float2(      0,  hOffset));  \n"
"    float4 ur = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 r  = texRECT(texture, coords + float2( wOffset,       0));  \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 b  = texRECT(texture, coords + float2(      0, -hOffset));  \n"
"    // scale by 8 to brighten the edges								\n"
"    return (16 * c - (bl + l + tl + t + ur + r + br + b)) / 8.0;		\n"
"}																		\n";

/*----------------------------------------------------------------------*/
// This shader performs a 9-tap Laplacian edge detection filter.
//
//	This is the actual cg code that will be compiled at runtime
//	and loaded into the graphic card, fragment processor
//
//	tex2D returns the value of the texture at coordinates
//
//	the laplacian filter implemented is a 3x3 filter with the 
//	following coefficients:
//
//		5	15	31		tl	t	tr
//		49	56	49		l	c	r
//		31	15	5		bl	b	br
//
//
static const char * GaussianShaderSource = 
"float4 Gaussian(float2 coords : TEXCOORD0,									\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;							\n"       
"    static const float hOffset = 1.0;							\n"    
"    float4 c  = texRECT(texture, coords);                              \n"
"    float4 bl = texRECT(texture, coords + float2(-wOffset, -hOffset)); \n"
"    float4 l  = texRECT(texture, coords + float2(-wOffset,       0));  \n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 t  = texRECT(texture, coords + float2(      0,  hOffset));  \n"
"    float4 tr = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 r  = texRECT(texture, coords + float2( wOffset,       0));  \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 b  = texRECT(texture, coords + float2(      0, -hOffset));  \n"
"    return (56.0 * c + 31.0 * bl + 49.0 * l + 5.0 * tl + 15.0 * t + 31.0 * tr + 49.0 * r + 5.0 * br + 15.0 * b) / 256.0;				\n"
"}																		\n";
