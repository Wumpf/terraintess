struct CHARVERTEX
{
    float2 Pos_pixel : POSITION;
	float Width_pixel : WIDTH;
    float2 TexPos_pixel : TEXCOORD;
};

CHARVERTEX main(CHARVERTEX input)
{
    return input;
}
