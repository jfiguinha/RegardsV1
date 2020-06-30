
#define FLOAT_FTOI_MAGIC_NUM (float) (3 << 21)
#define IT_FTOI_MAGIC_NUM 0x4ac00000

inline int FastFloatToInt(float f)
{
	f += FLOAT_FTOI_MAGIC_NUM;
	return (*((int *)&f) - IT_FTOI_MAGIC_NUM) >> 1;
}

inline int FloatToInt(float m_fValue)
{
	int iValue;
	_asm{
		fld m_fValue
		fistp iValue
	}

	return iValue;
}

inline WORD Changex87FPToTrunc(void)
{
	WORD NewCtrlWord, OldCtrlWord;
	_asm{
		fnstcw OldCtrlWord
		mov ax, OldCtrlWord
		or ax, 0c11h
		mov NewCtrlWord, ax
		fldcw NewCtrlWord
	}

	return OldCtrlWord;
}

inline WORD Setx87ControlWord(const WORD &NewCtrlWord)
{
	WORD OldCtrlWord;
	_asm{
		fnstcw OldCtrlWord
		fldcw NewCtrlWord
	}

	return OldCtrlWord;
}

