/* 
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/ 

#ifndef __TYANIME3DACOUSTICPATHFINDER__
#define __TYANIME3DACOUSTICPATHFINDER__

/**
 * \class TYANIME3DAcousticPathFinder
 * \brief La recherche de chemins acoustiques associee a la methode ANIME3D
 */
class TYANIME3DAcousticPathFinder
{
public:
    TYANIME3DAcousticPathFinder(TYStructSurfIntersect* tabPolygon, const size_t& tabPolygonSize, TYTabSourcePonctuelleGeoNode& tabSources, TYTabPointCalculGeoNode& tabRecepteurs, TYTabRay& tabTYRays);
    virtual ~TYANIME3DAcousticPathFinder();

	bool exec();

	/*!
	 * \fn void TYRayCorrection(TYRay& tyRay)
	 * \brief Curve TYRay with respect to meteo influence
	 *		  This is only for watching curved rays on screen
	 */
	void tyRayCorrection(TYRay *tyRay);

	Simulation& getRayTracer() { return _rayTracing; }

private :
    /*!
     * \fn unsigned int getTabsSAndR(const TYSite& site, TYCalcul& calcul, vector<OCoord3D>& sources, vector<OCoord3D>& recepteurs)
     * \brief Contruit la liste des points sources et des points recepteurs en fonction du parametre de sens de propa (S -> R) ou (R -> S)
     * \return Renvoie 0 pour le sens S->R et 1 pour le sens R->S
     */
    unsigned int getTabsSAndR(vector<vec3>& sources, vector<vec3>& recepteurs);

    /*!
     * \fn TYPoint computePosGlobalPoint (const TYGeometryNode* pNode);
     * \brief Calcule la position d'un point (source ou recepteur) dans le repere global
     */
    TYPoint computePosGlobalPoint(const TYGeometryNode* pNode);

    ///*!
    //* \fn void appendSourceToAnalyticRayTracer(TYCalcul &calcul, unsigned int nbRayons)
    //* \brief Ajoute les sources ponctuelles actives a la simulation
    //*/
    //void appendSourceToAnalyticRayTracer(vector<vec3>& tabSources);

    ///*!
    //* \fn void appendRecepteurToAnalyticRayTracer(TYCalcul& calcul, vector<vec3>& tabRecepteurs)
    //* \brief Ajoute les recepteurs poncutels actifs a la simulation
    //*/
    //void appendRecepteurToAnalyticRayTracer(vector<vec3>& tabRecepteurs);

    /*!
     * \fn void transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs)
     * \brief Adapte la position des sources et des recepteurs dans la geometrie transformee
     */
    void transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs);

    /*!
     * \fn vec3 modifGeom(const vec3&)
     * \brief modifie l'altimetrie d'un point en prenant en compte l'effet meteo
     */
    vec3 modifGeom(const vec3& point);

    /*!
    * \fn bool appendTriangleToScene()
    * \brief Convertion des triangles Tympan en primitives utilisables par le lancer de rayons.
    * \return Renvoie vrai si l'ensemble des primitives a bien pu etre importe dans la simulation.
    */
    bool appendTriangleToScene();

    /*!
    * \fn void appendRecepteurToSimulation(vector<vec3>& recepteurs)
    * \brief Ajoute les recepteurs a la simulation
    */
    void appendRecepteurToSimulation(vector<vec3>& recepteurs);

    /*!
    * \fn void appendSourceToSimulation(TYCalcul &calcul, vector<vec3>& sources)
    * \brief Ajoute les sources ponctuelles actives a la simulation
    */
    void appendSourceToSimulation(vector<vec3>& sources);

    /*!
    * \fn  void convertRaytoTYRay(const TYSiteNode& site, TYCalcul& calcul, const unsigned int& sens)
    * \brief convertion des rayons en rayons TYMPAN
    */
    void convertRaytoTYRay(const unsigned int& sens);

    /*!
     * \fn bool setSEtR(TYTabSourcePonctuelleGeoNode& sources, TYTabPointCalculGeoNode& tabRecepteurs, const unsigned int sens, TYRay& tyRay)
     * \brief affecte la source et le recepteur d'un rayon de type TYRay
     * \return Renvoie false si la source ou le recepteur ne sont pas trouves
     */
    bool setSEtRToTYRay(const unsigned int sens, Ray* r, TYRay* tyRay);

    /*!
     * \fn void buildListEvent(const int& sens, const Ray ray, std::list<TYEvent>& listEvent)
     * \brief construit une liste d'evenement TYEvent a partir d'un rayon
     */
    void buildListEvent(const int& sens, Ray* ray, std::vector<TYRayEvent*>& tabEvents);

	/*!
	 * \fn  void convertRayEventToTYRayEvent(const QSharedPoint<event> rev, TYRayEvent& e)
	 * \brief Convert an event (as defined by acousticRayTracer) to a TYEvent (used by solver)
	 */
	 void convertRayEventToTYRayEvent(const QSharedPointer<Event> rev, TYRayEvent* e);

    /*!
     * \fn void calculAnglesRayons(TYCalcul& calcul)
     * \brief Calcule les angles associes aux evenements des rayons
     */
    void calculeAngleTirRayon(TYRay* tyRay);

	/*!
	* \fn void sampleAndCorrection()
	* \brief Computes angle and length correction 
	* \ by calling the three previous functions
	* \ Creates two matrix which have corrected lengths and angles 
	*/
	void sampleAndCorrection();

	/*!
	 * \fn void endLenghtCompute(TYRay *tyRay)
	 * \brief compute the length between an event and the next pertinent event
	 *		  (i.e. betwween a diffraction and the next reflection or receptor)
	 */
	void endLenghtCompute(TYRay *tyRay);

	/*!
	 * \fn void angleCompute(TYRay *tyRay)
	 * \brief compute the angle between incident ray and the face 
	 */
	void angleCompute(TYRay *tyRay);

	/*!
	 * \fn void nextLenghtCompute(TYRay *tyRay)
	 * \brief compute the length between an event and the next event
	 */
	void nextLenghtCompute(TYRay *tyRay);

	/*!
	 * \fn void prevNextLengthCompute(TYRay *tyRay)
	 * \brief Computes the length between event-1 and event+1
	 */
	void prevNextLengthCompute(TYRay *tyRay);

	/*!
	 * \fn void nextLenghtCompute(TYRay *tyRay)
	 * \brief compute the length between an event and the next event
	 */
	void eventPosCompute(TYRay *tyRay);

	/*!
	* \fn void lengthCorrection()
	* \brief Computes length correction on path
	* \ Works on over-sampled TYRays
	* \ Creates a vector which has all corrected path lengths
	*/
	double lengthCorrection(TYRayEvent *ev1, const TYRayEvent *ev2);

	/*!
	* \fn void angleCorrection()
	* \brief Computes angle correction on path
	*		 ev1 -> previous event
	*		 ev2 -> event to wich compute anngle
	*		 ev3 -> next event
	*/
	double angleCorrection(const TYRayEvent *ev1, TYRayEvent *ev2, const TYRayEvent *ev3);

private:
    /// Objet _rayTracing pour le lancer de rayons droits
    Simulation _rayTracing;

    /// Objet _curveRayTracing pour le lancer de rayons courbes
    Transfo _curveRayTracing;

	    /// Tableau contenant l'ensemble des infos relatives a la geometrie d'un site et les materiaux associes a chaque face
    TYStructSurfIntersect* _tabPolygon;

    /// Nombre de polygones presents dans _tabPolygon
    const size_t& _tabPolygonSize;

   	/*!< List of sources used by the solver */
    TYTabSourcePonctuelleGeoNode& _tabSources;

	/*!< List of receptors used by the solver */
	TYTabPointCalculGeoNode& _tabRecepteurs;
	
	/// tableau de l'ensemble des rayons métier Code_TYMPAN
    TYTabRay& _tabTYRays;
};

#endif // __TYANIME3DACOUSTICPATHFINDER__
