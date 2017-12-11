.. _dev-datamodel:

Python Data Model
=================

   **Source and Receptor classes**

   .. graphviz::

      digraph "classes" {
      charset="utf-8"
      rankdir=BT
      "0" [label="{Source|position\lspectrum\ldirectivity\l|}", shape="record"];
      "1" [label="{Receptor|position\l_spectrum\l|spectrum()}", shape="record"];
      }


   **Infrastructure interface**

   .. graphviz::

      digraph "classes" {
      charset="utf-8"
      rankdir=BT
      "0" [label="{Building|\l|}", shape="record"];
      "1" [label="{Infrastructure|is_emitting : bool\lname : str\l|export_mesh()\lmaterial_by_face()\l}", shape="record"];
      "2" [label="{Machine|\l|}", shape="record"];
      "3" [label="{Screen|\l|}", shape="record"];
      "0" -> "1" [arrowhead="empty", arrowtail="none"];
      "2" -> "1" [arrowhead="empty", arrowtail="none"];
      "3" -> "1" [arrowhead="empty", arrowtail="none"];
      }


