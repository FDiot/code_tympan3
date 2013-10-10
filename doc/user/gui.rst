.. _user-gui:

Graphical User Interface
========================

Qt4 interface.

Presentation
------------

.. todo::

   - explain how the GUI is organized
   - Maybe a very simple "How to"

     - how can I load a project
     - how can I add a site/building/source etc
     - how can I change views
     - how can I launch a simulation
     - how can I see results


.. _user-gui-editing:

Building / Editing
------------------

.. todo::

   - List of icons/buttons to create and edit some objects
   - List of related keybindings if they exist

PolyLine editing
````````````````

In editing mode, and after selecting the polyline, the user can enter in
"insert mode" by pressing the **K** Key. When the left button of the
mouse will be pressed, a new point will be created on the closest segment
of the polyline.

Alternatively, you can delete a point of the polyline by pressing the **D** key and the left button of the mouse.

Editiong a Palette
``````````````````

When editing a Palette, i.e. the color mapping of noise levels for
charts, you can now :

* edit **both** the *upper bound* for a color and the color itself
* add or delete intermediary bounds and colors
* load or save a palette to / from a file or reset it to the default
* initialize a regular palette

This last operation uses the lowest and highest bounds for the noise level
as well as the number of color of the current palette. Then it
resamples linearly the interval between the bounds and associate to
them auto-generated color between the lower bound's color and the *and
more* color.

For example if you want a coarse 5 color palette blue to red for
values from less than 40 dB up to more than 100 dB :

* you add/remove lines until you have 5
* you set the lowest bound to 40 and the highest to 100
* you set the color for 40 to blue
* you set the color for *and more* to red
* you press the button *generated linear palette*

The values of the bounds will be resampled to 40, 60, 80 and 100 dB
and the colors will range from blue to red through purple.


.. _user-gallery:

Gallery
-------

.. todo::

   Put some pretty screeshots here with a legend

   **note** images can be accessed from root source folder as
    ``/_static/path-to-image``

