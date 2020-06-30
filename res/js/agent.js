///////////////////////////////////////////////////////////////////
//Texte � dire
///////////////////////////////////////////////////////////////////

var m_ctlAgentActiveX;
var merlin;
var iAide = 0;
var iLastId = 0;

var m_stgCadreFormulaire = "Ce cadre contient un formulaire permettant de d�finir les crit�res de votre recherche";
var m_stgListeCritere = "Cette liste d�roulante contient les crit�res d�finissant vos images. Ces crit�res peuvent �tre cr�es et modifi�s dans le menu Album/Critere.";
var m_stgEditLabel = "Cette boite d'�dition permet de d�finir les caract�res qui doivent �tre recherch� dans l'image.";
var m_stgBoutonRecherche = "Appuyer sur ce bouton pour lancer la recherche.";
var m_stgBoutonEtCheck = "Cocher la case Et force le moteur de recherche � rechercher les images contenant l'ensemble des crit�res d�finis.";
var m_stgBoutonOuCheck = "Cocher la case Ou force le moteur de recherche � rechercher les images contenant au mois un des crit�res d�finis.";

var m_stgCadreRechercheAvance = "Cette boite de dialogue permet de d�finir des crit�res de recherche plus pr�cis sur l'image.";
var m_stgBoutonAffichage = "En appuyant sur cette image vous pouvez faire apparaitre ou disparaitre cette boite de dialogue.";
var m_stgLibelleDateDe = "Cette boite d'�dition permet de d�finir la date au plus t�t.";
var m_stgLibelleDateA = "Cette boite d'�dition permet de d�finir la date au plus tard.";
var m_stgTypeImage = "Cette liste d�roulante permet de choisir le type de l'image recherch�e.";
var m_stgCalendrier = "Cette image permet de faire appara�tre un calendrier.";
var m_stgTaille = "Cette boite d'�dition permet d'indiquer en kilo octet la taille de l'image.";
var m_stgListeTaille = "Cette liste d�roulante sert � indiquer la taille minimale ou maximale d'une image.";
var m_stgHauteur = "Cette boite d'�dition permet d'indiquer la taille en pixel de la hauteur de l'image.";
var m_stgLargeur = "Cette boite d'�dition permet d'indiquer la taille en pixel de la largeur de l'image.";

var m_stgAide = "En cliquant sur ce lien vous ferez apparaitre l'aide de Regards.";
var m_stgAssistant = "Ce lien me fera appara�tre. Cliquer dessus pour me faire dispara�tre.";
var m_stgPresentation = "Bonjour, Je suis Merlin votre Assistant d'aide � la recherche. Positionnez moi devant un �l�ment pour obtenir une explication.";

function testActivexAgent()
{
 	if(iAide == 0)
 	{
		m_ctlAgentActiveX = new ActiveXObject("Agent.Control.2");
		if (m_ctlAgentActiveX)
		{
			 m_ctlAgentActiveX.Connected = true;
			 m_ctlAgentActiveX.Characters.Load("Merlin", "merlin.acs");
			 merlin = m_ctlAgentActiveX.Characters.Character("Merlin");
			 merlin.MoveTo(iMouseX,iMouseY);
			 merlin.LanguageID = 0x040C;
			 merlin.Show();
			 merlin.Speak(m_stgPresentation);
			 //merlin.Think("I beg to make no mistake yet!");
	      	}
	      	else
	      	{
	      		if(confirm("Veuillez t�l�charger le fichier genie.exe sur le site http://agent.microsoft.com"))
	      			window.open("http://agent.microsoft.com","_new");	
	      	}
	      iAide = 1;
	}
	else
	{
		merlin.hide();
		iAide = 0;
	}
	
}

function stopActivex()
{
	if(iAide) 
		merlin.Stop();
}

function affichageAide(m_stgLibelle, id)
{
	if(iAide && (iLastId != id))
	{
		merlin.Stop();
		merlin.MoveTo(iMouseX,iMouseY);
		merlin.Speak(m_stgLibelle);
		iLastId = id;
	}
}