#include "stdafx.h"
#include ".\imagecalaloguerecherche.h"
#include <SADirRead.h>
#include "loadimage.h"
#include <ibitmap.h>
#include "security.h"
#include "filtre.h"
#include "objet.h"
#include <CatalogueData.h>
#include <ItemInfo.h>

CImageCalalogueRecherche::CImageCalalogueRecherche(void)
{
	iTabAlbumSrc = NULL;
	iNbAlbumSrc = 0;
	iTabAlbumDest = NULL;
	iNbAlbumDst = 0;
	m_CatalogueData = NULL;
}

CImageCalalogueRecherche::~CImageCalalogueRecherche(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCalalogueRecherche::TraitementParenthese(int * iResult,int * iOp,int &j,int &i, const int& iNbOp, const int &iNbCritere)
{
	int iCorrespondant = iResult[j-1];
	
	for(;i < iNbOp;i++,j++)
	{
		switch(iOp[i])
		{
			case 2:
				i++;
				iCorrespondant = TraitementParenthese(iResult,iOp,j,i,iNbOp,iNbCritere);
				break;

			case 3:
				return iCorrespondant;

			case 4:
				{
					if(i + 1 < iNbOp)
					{
						if(iOp[i+1] == 2)
						{
							i++;

							if(iCorrespondant != 1)
								iCorrespondant = 0;
							else
							{
								if(TraitementParenthese(iResult,iOp,j,i,iNbOp,iNbCritere) != 1)
									iCorrespondant = 0;
							}

							continue;
						}
					}

					if(iResult[j] != 1 && iCorrespondant != 1)
							iCorrespondant = 0;
				}
				break;

			case 5:
				{
					if(i + 1 < iNbOp)
					{
						if(iOp[i+1] == 2)
						{
							i++;
							if(iCorrespondant != 1)
							{
								if(TraitementParenthese(iResult,iOp,j,i,iNbOp,iNbCritere) != 1)
									iCorrespondant = 0;
								else
									iCorrespondant = 1;
							}
							else
								TraitementParenthese(iResult,iOp,j,i,iNbOp,iNbCritere);

							continue;
						}
					}

					if(iCorrespondant != 1)
					{
						if(iResult[j] != 1)
							iCorrespondant = 0;
						else
							iCorrespondant = 1;
					}
				}
				break;
		}

	}

	return iCorrespondant;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCalalogueRecherche::DataComparaison(CIconeFile &m_IconeFile,IconeFileVector &m_IconeFileVectorSrc,IconeFileVector &m_IconeFileVectorDst)
{
	IconeFileVector::iterator fit;

	for (fit = m_IconeFileVectorSrc.begin(); fit!= m_IconeFileVectorSrc.end(); fit++)
	{
		//IconeFile * m_IconeFileLocal = fit;

		IconeFileVector::iterator m_IconeFileLocal = fit;

		string m_stgChaine;
		int iPos = 0;
		int iOldPos = 0;
		
		int iResult[20];
		int iResultCompteur = 0;
		int iOp[20];
		int iOpCompteur = 0;
		int iCorrespondant = 0;
		////////////////////////////////////////////////////////////////////
		//Comparaison des données
		////////////////////////////////////////////////////////////////////

		//Analyse des paramètres à comparer

		do
		{
			iPos = m_stgChaineRecherche.find(";",iPos);
			if(iPos != -1)
			{
				int iLocalPos = 0;
				m_stgChaine = m_stgChaineRecherche.substr(iOldPos,iPos - iOldPos);

				iLocalPos = m_stgChaine.find("op=");
				if(iLocalPos != -1)
				{
					iLocalPos += 3;

					iOp[iOpCompteur] = atoi(m_stgChaine.substr(iLocalPos,m_stgChaine.length() - iLocalPos).c_str());

					iOpCompteur++;

				}

				iLocalPos = m_stgChaine.find("critere=");
				if(iLocalPos != -1)
				{
					iLocalPos += 8;

					int iNumCritere = atoi(m_stgChaine.substr(iLocalPos,m_stgChaine.length() - iLocalPos).c_str());

					switch(iNumCritere)
					{
						case 0:
							{
								char TempLocalFileNameSrc[MAX_PATH];
								char TempLocalFileNameDst[MAX_PATH];
								CObjet m_cObjet;
								//Critere 0 -> Comparaison des noms

								m_cObjet.GetFileName(m_IconeFileLocal->m_stgFullPathName,TempLocalFileNameSrc,MAX_PATH);
								m_cObjet.GetFileName(m_IconeFile.m_stgFullPathName,TempLocalFileNameDst,MAX_PATH);

								if(strcmp(TempLocalFileNameSrc,TempLocalFileNameDst) == 0)
									iResult[iResultCompteur] = 1;
								else
									iResult[iResultCompteur] = 0;

								iResultCompteur++;
							}
							break;

						case 1:
							{
								//Critere 1 -> Comparaison des dates
								if(strcmp(m_IconeFileLocal->szTimeCreate,m_IconeFile.szTimeCreate) == 0)
									iResult[iResultCompteur] = 1;
								else
									iResult[iResultCompteur] = 0;

								iResultCompteur++;
							}
							break;

						case 2:
							{
								//Critere 2 -> Comparaison de la taille
								if(strcmp(m_IconeFileLocal->szSize,m_IconeFile.szSize) == 0)
									iResult[iResultCompteur] = 1;
								else
									iResult[iResultCompteur] = 0;

								iResultCompteur++;
							}
							break;

						case 3:
							{
								//Critere 3 -> Comparaison de la signature
								string m_stgChaineLocal = m_IconeFileLocal->informations;
								string m_stgChaineSignature;
								int iLocalPos = m_stgChaineLocal.find("signature=");
								iLocalPos+=10;
								int iLocalFin = m_stgChaineLocal.find(";",iLocalPos);

								m_stgChaineSignature = m_stgChaineLocal.substr(iLocalPos,iLocalFin-iLocalPos);

								m_stgChaineLocal = m_IconeFile.informations;

								int i = m_stgChaineLocal.find(m_stgChaineSignature);

								if(i != -1)
									iResult[iResultCompteur] = 1;
								else
									iResult[iResultCompteur] = 0;

								iResultCompteur++;
							}
							break;

						case 4:
							{
								//Critere 4 -> Comparaison de la hauteur et la largeur

								string m_stgChaineLocal = m_IconeFileLocal->informations;
								string m_stgChaineDimension;
								int iLocalPos = m_stgChaineLocal.find("dimension=");
								iLocalPos+=10;
								int iLocalFin = m_stgChaineLocal.find(";",iLocalPos);

								m_stgChaineDimension = m_stgChaineLocal.substr(iLocalPos,iLocalFin-iLocalPos);

								m_stgChaineLocal = m_IconeFile.informations;

								if(m_stgChaineLocal.find(m_stgChaineDimension) != -1)
									iResult[iResultCompteur] = 1;
								else
									iResult[iResultCompteur] = 0;

								iResultCompteur++;

							}
							break;

						case 5:
							{
								//Critere 5 -> Comparaison du format

								int iFormat1, iFormat2;
								CObjet m_cObjet;

								iFormat1 = m_cObjet.TestImageFormat(m_IconeFileLocal->m_stgFullPathName);
								iFormat2 = m_cObjet.TestImageFormat(m_IconeFile.m_stgFullPathName);
								
								if(iFormat1 == iFormat2)
									iResult[iResultCompteur] = 1;
								else
									iResult[iResultCompteur] = 0;

								iResultCompteur++;

							}
							break;
					}
				}
				iPos++;
				iOldPos=iPos;
			}
			

		}while(iPos != -1);


		int iParantheseOuv = 0;
		int i,j = 1;
		iCorrespondant = iResult[0];
		
		for(i = 0;i < iOpCompteur;i++)
		{
			switch(iOp[i])
			{
				case 2:
					i++;
					iCorrespondant = TraitementParenthese(iResult,iOp,j,i,iOpCompteur,iResultCompteur);
					break;

				case 4:
					{
						if(i + 1 < iOpCompteur)
						{
							i++;
							if(iOp[i] == 2)
							{
								if(iCorrespondant != 1)
									iCorrespondant = 0;
								else
								{
									if(TraitementParenthese(iResult,iOp,j,i,iOpCompteur,iResultCompteur) != 1)
										iCorrespondant = 0;
								}
							}
						}
						else
						{
							if(iResult[j] != 1 && iCorrespondant != 1)
								iCorrespondant = 0;
						}
						j++;
					}
					break;

				case 5:
					{

						if(i + 1 < iOpCompteur)
						{
							i++;
							if(iOp[i] == 2)
							{
								if(iCorrespondant != 1)
								{
									if(TraitementParenthese(iResult,iOp,j,i,iOpCompteur,iResultCompteur) != 1)
										iCorrespondant = 0;
									else
										iCorrespondant = 1;
								}
								else
									TraitementParenthese(iResult,iOp,j,i,iOpCompteur,iResultCompteur);
							}
						}
						else
						{
							if(iCorrespondant != 1)
							{
								if(iResult[j] != 1)
									iCorrespondant = 0;
								else
									iCorrespondant = 1;

							}
						}
						j++;
					}
					break;
			}

			if(iCorrespondant == 0)
				break;
		}


		if(iCorrespondant == 1)
		{
			CAttribut m_Attribut;
			StringCchCopy(m_Attribut.cLibelle,MAX_PATH,m_IconeFile.m_stgFullPathName);
			m_Attribut.iNumAttribut = m_IconeFileLocal->m_AttributVector.size() + 1;
			m_IconeFileLocal->m_AttributVector.push_back(m_Attribut);
		}

	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCalalogueRecherche::GenerateHTMLFile(IconeFileVector &m_IconeFileVectorSrc, char * m_cHTMLFile)
{
	////////////////////////////////////////////////////
	//Génération du fichier HTML 
	////////////////////////////////////////////////////

	char Temp[2048];

	IconeFileVector::iterator fit;
	AttributVector::iterator dit;


	//Création du fichier HTML

	//StringCchPrintf(m_cHTMLFile,"%s\\rapport.html",m_CatalogueData->cDirectory);

	//if(!DeleteFile(m_cHTMLFile))
	//{
		//Generation d'un nom de fichier unique
		SYSTEMTIME SystemTime;

		GetSystemTime(&SystemTime);

		StringCchPrintf(m_cHTMLFile,MAX_PATH,"%s\\rapport",m_CatalogueData->m_cDefaultDirectory);

		if(!CObjet::TestRepertoire(m_cHTMLFile))
			CreateDirectory(m_cHTMLFile,NULL);

		StringCchPrintf(m_cHTMLFile,MAX_PATH,"%s\\rapport\\%d%d%d%d%d%d.html",m_CatalogueData->m_cDefaultDirectory, SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
			SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

		//StringCchPrintf(m_cHTMLFile, "%s\\%s.html",m_CatalogueData->cDirectory, Temp);

	//}

	int fichier_cible = 0;
	LoadString(hInstance,IDS_ENTETEHTMLCATALGOUE,Temp,2048);

	if(_sopen_s(&fichier_cible,m_cHTMLFile,_O_WRONLY | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;
		if(fichier_cible == -1)
			return -1;
	_write(fichier_cible, Temp,  strlen(Temp));

	for (fit = m_IconeFileVectorSrc.begin(); fit!= m_IconeFileVectorSrc.end(); fit++)
	{
		IconeFileVector::iterator m_IconeFileLocal = fit;
		//IconeFile * m_IconeFileLocal = fit;

		if(m_IconeFileLocal->m_AttributVector.size() > 0)
		{
			//Ecriture du champ HTML
			string m_stgChaine = "<center><table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#000000\" width=\"90%\" id=\"AutoNumber1\">";
			_write(fichier_cible, m_stgChaine.c_str(),  m_stgChaine.length());

			m_stgChaine = "<tr><td width=\"70%\" bgcolor=\"#DEF8E9\"><font size=\"2\">Fichier : </font>";
			_write(fichier_cible, m_stgChaine.c_str(),  m_stgChaine.length());

			StringCchPrintf(Temp,2048,"<a href=\"file:///%s\"><font size=\"2\" color=\"#000000\">%s</font></a></td>",m_IconeFileLocal->m_stgFullPathName,m_IconeFileLocal->m_stgFullPathName);
			_write(fichier_cible, Temp,  strlen(Temp));

			m_stgChaine = "<td width=\"30%\" bgcolor=\"#DEF8E9\">";
			m_stgChaine += "<p style=\"text-align: center\"><b><font size=\"2\">Photo de prévisualisation</font></b></td>";
			m_stgChaine += "</tr><tr><td width=\"70%\" bgcolor=\"#FDDEFE\"><font size=\"2\">Liste des fichiers liés :</font></td>";
			
			_write(fichier_cible, m_stgChaine.c_str(),  m_stgChaine.length());

			//Récupération des informations sur la taille du fichier
			string m_stgChaineLocal = m_IconeFileLocal->informations;
			string m_stgChaineDimension;
			int iLocalPos = m_stgChaineLocal.find("dimension=");
			iLocalPos+=10;
			int iLocalFin = m_stgChaineLocal.find(";",iLocalPos);

			m_stgChaineDimension = m_stgChaineLocal.substr(iLocalPos,iLocalFin-iLocalPos);

			iLocalFin = m_stgChaineDimension.find("x",0);

			int iWidth = atoi(m_stgChaineDimension.substr(0,iLocalFin).c_str());
			iLocalFin++;
			int iHeight = atoi(m_stgChaineDimension.substr(iLocalFin,m_stgChaineDimension.length() - iLocalFin).c_str());

			float fRapport = iHeight / 70.0f;

			iWidth = iWidth / fRapport;

			StringCchPrintf(Temp,2048,"<td align=\"center\" rowspan=\"%d\"><img border=\"0\" src=\"%s\" width=\"%d\" height=\"70\"></td></tr>",m_IconeFileLocal->m_AttributVector.size()+1,
				m_IconeFileLocal->m_stgFullPathName,iWidth);

			_write(fichier_cible, Temp,  strlen(Temp));

			for (dit = m_IconeFileLocal->m_AttributVector.begin(); dit!= m_IconeFileLocal->m_AttributVector.end(); dit++)
			{
				//Attribut * m_Attribut = dit;
				StringCchPrintf(Temp,2048,"<tr><td width=\"70%\"><a href=\"file:///%s\"><font size=\"2\">%s</font></a></td></tr>",dit->cLibelle,dit->cLibelle);
				_write(fichier_cible, Temp,  strlen(Temp));

			}

			m_stgChaine = "</table></center><br>";
			_write(fichier_cible, m_stgChaine.c_str(),  m_stgChaine.length());

			//Ajout dans le vecteur de recherche
			m_CatalogueData->m_IconeFileRecherche.push_back(*m_IconeFileLocal);
		}
	}


	StringCchCopy(Temp,2048,"</body></html>");

	_write(fichier_cible, Temp,  strlen(Temp));

	_close(fichier_cible);

	return 0;
}




////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int CImageCalalogueRecherche::ShowHTML(TCHAR * szTemp,const int &iTempSize,const int &iWidth, const int &iHeight)
{
	HINSTANCE   hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL"));

	if(hinstMSHTML)
	   {
	   SHOWHTMLDIALOGFN  *pfnShowHTMLDialog;
      
	   pfnShowHTMLDialog = (SHOWHTMLDIALOGFN*)GetProcAddress(hinstMSHTML, TEXT("ShowHTMLDialog"));

	   if(pfnShowHTMLDialog)
		  {
		  IMoniker *pmk;
		  OLECHAR  bstr[MAX_PATH*2];

		  CObjet::LocalToBSTR(bstr, szTemp, ARRAYSIZE(bstr));

		  CreateURLMoniker(NULL, bstr, &pmk);

		  if(pmk)
			 {
			 HRESULT  hr;
			 VARIANT  varArgs, varReturn;
			 wchar_t cOptions[255];
			 char Temp[255];

			 StringCchPrintf(Temp,255,"center:yes;resizable:yes;scroll:yes;status:yes;dialogWidth:%dpx;dialogHeight:%dpx;",iWidth,iHeight);

			 //mbstowcs(cOptions, Temp, 255);
			 size_t m_sizeTConvert;
			 mbstowcs_s(&m_sizeTConvert,cOptions,255, Temp, 255);


         
			 VariantInit(&varReturn);

			 varArgs.vt = VT_BSTR;
			 varArgs.bstrVal = SysAllocString(L"Argument 1;Argument 2;Argument 3;Argument 4");


			 hr = (*pfnShowHTMLDialog)(NULL, pmk, &varArgs, cOptions, &varReturn);

			 VariantClear(&varArgs);
         
			 pmk->Release();

			 if(SUCCEEDED(hr))
			 {
				switch(varReturn.vt)
				   {
					   case VT_BSTR:
						  {
							  TCHAR szData[MAX_PATH];

							  CObjet::BSTRToLocal(szData, varReturn.bstrVal, ARRAYSIZE(szData));

							  StringCchPrintf(szTemp,iTempSize, TEXT("The selected item was \"%s\"."), szData);

							  VariantClear(&varReturn);
						  }
						  break;

					   default:
						  StringCchCopy(szTemp,iTempSize,TEXT("Cancel was selected."));
						  break;
				   }
				}           
			 }
		  }
	   FreeLibrary(hinstMSHTML);
	   }

	return 0;
}




