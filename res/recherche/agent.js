///////////////////////////////////////////////////////////////////
//Texte à dire
///////////////////////////////////////////////////////////////////

var m_ctlAgentActiveX;
var merlin;
var iAide = 0;
var iLastId = 0;

var m_stgCadreFormulaire = "Ce cadre contient un formulaire permettant de définir les critères de votre recherche";
var m_stgListeCritere = "Cette liste déroulante contient les critères définissant vos images. Ces critères peuvent être crées et modifiés dans le menu Album/Critere.";
var m_stgEditLabel = "Cette boite d'édition permet de définir les caractères qui doivent être recherché dans l'image.";
var m_stgBoutonRecherche = "Appuyer sur ce bouton pour lancer la recherche.";
var m_stgBoutonEtCheck = "Cocher la case Et force le moteur de recherche à rechercher les images contenant l'ensemble des critères définis.";
var m_stgBoutonOuCheck = "Cocher la case Ou force le moteur de recherche à rechercher les images contenant au mois un des critères définis.";

var m_stgCadreRechercheAvance = "Cette boite de dialogue permet de définir des critères de recherche plus précis sur l'image.";
var m_stgBoutonAffichage = "En appuyant sur cette image vous pouvez faire apparaitre ou disparaitre cette boite de dialogue.";
var m_stgLibelleDateDe = "Cette boite d'édition permet de définir la date au plus tôt.";
var m_stgLibelleDateA = "Cette boite d'édition permet de définir la date au plus tard.";
var m_stgTypeImage = "Cette liste déroulante permet de choisir le type de l'image recherchée.";
var m_stgCalendrier = "Cette image permet de faire apparaître un calendrier.";
var m_stgTaille = "Cette boite d'édition permet d'indiquer en kilo octet la taille de l'image.";
var m_stgListeTaille = "Cette liste déroulante sert à indiquer la taille minimale ou maximale d'une image.";
var m_stgHauteur = "Cette boite d'édition permet d'indiquer la taille en pixel de la hauteur de l'image.";
var m_stgLargeur = "Cette boite d'édition permet d'indiquer la taille en pixel de la largeur de l'image.";

var m_stgAide = "En cliquant sur ce lien vous ferez apparaitre l'aide de Regards.";
var m_stgAssistant = "Ce lien me fera apparaître. Cliquer dessus pour me faire disparaître.";
var m_stgPresentation = "Bonjour, Je suis Merlin votre Assistant d'aide à la recherche. Positionnez moi devant un élément pour obtenir une explication.";

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
	      		if(confirm("Veuillez télécharger le fichier genie.exe sur le site http://agent.microsoft.com"))
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