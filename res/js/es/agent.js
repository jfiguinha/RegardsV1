///////////////////////////////////////////////////////////////////
//Texte � dire
///////////////////////////////////////////////////////////////////

var m_ctlAgentActiveX;
var merlin;
var iAide = 0;
var iLastId = 0;

var m_stgCadreFormulaire = "Este cadro contiene un formulario que permite definir los criterios de su investigaci�n.";
var m_stgListeCritere = "Esta lista que desenrolla contiene los criterios que definen sus im�genes. Estos criterios pueden crearse y modificarse en el peque�o �lbum/Criterio.";
var m_stgEditLabel = "Esta caja permite definir los caracteres para buscar imagenes.";
var m_stgBoutonRecherche = "Apoyar en este bot�n para lanzar la investigaci�n.";
var m_stgBoutonEtCheck = "Cochero la casilla Y fuerza el motor de investigaci�n a buscar las im�genes que contienen todos los criterios definidos.";
var m_stgBoutonOuCheck = "Cochero la casilla O fuerza el motor de investigaci�n a buscar las im�genes que contienen al menos uno de los criterios definidos.";

var m_stgCadreRechercheAvance = "Esta cojea de di�logo permite definir criterios de investigaci�n m�s precisos sobre la imagen.";
var m_stgBoutonAffichage = "Al apoyar en esta imagen pueden hacer appara o dispara esta cojea de di�logo.";
var m_stgLibelleDateDe = "Esta caja permite de definir la fecha al menos.";
var m_stgLibelleDateA = "Esta caja permite de definir la fecha al mas tarde.";
var m_stgTypeImage = "Esta lista que desenrolla permite elegir el tipo de imagen que busque.";
var m_stgCalendrier = "Esta imagen permite evidenciar un calendario.";
var m_stgTaille = "Esta caja permite de especificar en kilo octeto el tama�o de la imagen.";
var m_stgListeTaille = "Esta lista que desenrolla sirve para indicar el tama�o m�nimo o m�ximo de la imagen.";
var m_stgHauteur = "Esta caja permite de especificar el tama�o en pixel de la altura de la imagen.";
var m_stgLargeur = "Esta caja permite de especificar el tama�o en pixel de la anchura de la imagen.";

var m_stgAide = "Al presionar este v�nculo har�n la ayuda de Regards.";
var m_stgAssistant = "Este v�nculo me evidenciar�. Pulsar arriba para hacerme desaparecer.";
var m_stgPresentation = "Buenos d�as, soy Merlin su Ayudante por la investigaci�n. Coloq�e me al frente de un elemento para obtener una explicaci�n.";

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