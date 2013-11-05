.. _ongoingwork-part:

Ongoing work
############

Factorisation of altimetry updates
==================================

Please refer to ticket [1484188]_.

This section summarise which altimetry update snippets are found where
in the code and when they are invoked. This aims at factoring them out
and rationalise altimetry update around as few code variants and
semantic invariants as possible.

Edition of topography elements
------------------------------

In the classes :class:`TYCourbeNiveauEditor`,
:class:`TYEmpriseEditor`, :class:`TYPlanEauEditor` we find the same
altimetry update pattern.

In the :func:`slotKeyPressed` methods of the tree classes:

.. code-block:: cpp

    ((TYSiteModelerFrame*)_pModeler)->getSite()->updateAltimetrie();
    ((TYSiteModelerFrame*)_pModeler)->getSite()->updateAltiInfra();
    ((TYSiteModelerFrame*)_pModeler)->getSite()->updateGraphic();
    _pInteractor->updateGL(); // ??? to be excluded

In the :func:`TYCourbeNiveauEditor::endCourbeNiveau` and
in the :func:`TYPlanEauEditor::endPlanEau` (**but not**
in the :func:`TYEmpriseEditor::endEmprise`):

.. code-block:: cpp

    // On altimetrise aussi le sous-site (si s'en est un !)
    if (!pSite->getRoot()) { pSite->updateAltimetrie(true); }

    // On met a jour l'altimetrie globale du site
    TYProjet* pProjet = getTYApp()->getCurProjet();
    if (pProjet)
    {
        pProjet->getSite()->updateAltimetrie(true);
        pProjet->getSite()->updateAltiInfra(true);
        pProjet->updateAltiRecepteurs();
        pProjet->getSite()->getTopographie()->updateGraphicTree();
    }

In contrast in :func:`TYSolEditor::endSol` methods:

.. code-block:: cpp

    for (unsigned int i = 0; i < tabPts.size(); i++)
    {
        tabPts[i]._z = 0.0;
        pSite->getTopographie()->getAltimetrie()->updateAltitude(tabPts[i]);
    }

.. warning:: The main display problem for ground material is likely to
             come from here !!!

With receptors update
---------------------

In :func:`TYPickEditor::showPopupMenu`:

.. code-block:: cpp

    // On altimetrise aussi le sous-site (si s'en est un !)
    if (pSiteParent && !pSiteParent->getRoot()) { pSiteParent->updateAltimetrie(true); }

    // On met a jour l'altimetrie globale du site
    TYProjet* pProjet = getTYApp()->getCurProjet();
    if (pProjet)
    {
        pProjet->getSite()->updateAltimetrie(true);
        pProjet->getSite()->updateAltiInfra(true);
        pProjet->updateAltiRecepteurs();

        pProjet->getSite()->getTopographie()->updateGraphicTree();
    }

is very similar to (in :func:`TYSiteFrame::contextMenuEvent`):

.. code-block:: cpp

    if (pSiteParent)   // Non NULL uniquement pour les courbes de niveau et les plan d'eau
    {
        // On altimetrise aussi le sous-site (si s'en est un !)
        // WIP
        if (!pSiteParent->getRoot()) { pSiteParent->updateAltimetrie(true); }

        // On met a jour l'altimetrie globale du site
        TYProjet* pProjet = getTYApp()->getCurProjet();
        if (pProjet)
        {
            pProjet->getSite()->updateAltimetrie(true);
            pProjet->getSite()->updateAltiInfra(true);
            pProjet->updateAltiRecepteurs();

            pProjet->getSite()->getTopographie()->updateGraphicTree();
        }
    }


Only updateAltimetry()
----------------------

In :func:`TYOpenElementDialog::openElement`:

.. code-block:: cpp

    // Directement site courant, la "place" etant libre
    LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElt);
    if (pSite)
    {
        getTYApp()->setCurSiteNode(pSite);
        pSite->updateAltimetrie();
    }

The most complex one
--------------------

In :func:`TYSiteNode::update`:

.. code-block:: cpp

    // Mise a jour de l'altimetrie du site principal
    updateAltimetrie(force);

    // Altimetrisation des infrastructures du site
    updateAltiInfra(force);

    // Mise a jour de l'acoustique des elements du site
    updateAcoustique(force);

    // Et celle des sites inclus
    for (unsigned short i = 0; i < _listSiteNode.size(); i++)
    {
        TYSiteNode* pSite = TYSiteNode::safeDownCast(_listSiteNode[i]->getElement());

        if (pSite && pSite->isInCurrentCalcul()) { pSite->update(force); }
    }

    // Si le site est dans un projet, on altimétrise les points de controle
    if (_pProjet)
    {
        TYCalcul* pCalcul = _pProjet->getCurrentCalcul()._pObj;
        assert(pCalcul);
        pCalcul->updateAltiRecepteurs();
    }


.. [1484188] https://extranet.logilab.fr/ticket/1484188


Documentation's TODOs
=====================

This section lists all ``..todo::`` markers through the developers'
documentation.

.. todolist::
