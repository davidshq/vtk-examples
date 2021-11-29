#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkRenderingCore import (
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextActor
)


def main():
    colors = vtkNamedColors()

    # Create a rendering window and renderer.
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.SetWindowName('TextActor')
    renWin.AddRenderer(ren)

    # Create a render window interactor.
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create a text actor.
    txt = vtkTextActor()
    txt.SetInput('Hello World!')
    txtprop = txt.GetTextProperty()
    txtprop.SetFontFamilyToArial()
    txtprop.BoldOn()
    txtprop.SetFontSize(36)
    txtprop.ShadowOn()
    txtprop.SetShadowOffset(4, 4)
    txtprop.SetColor(colors.GetColor3d('Cornsilk'))
    txt.SetDisplayPosition(20, 30)

    # Assign actor to the renderer.
    ren.AddActor(txt)
    ren.SetBackground(colors.GetColor3d('DarkGreen'))

    # Enable user interface interactor.
    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()