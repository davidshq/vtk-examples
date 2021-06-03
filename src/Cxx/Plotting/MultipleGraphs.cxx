#include <vtkAxis.h>
#include <vtkBrush.h>
#include <vtkChartXY.h>
#include <vtkContextActor.h>
#include <vtkContextScene.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkPlotPoints.h>
//#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

#include <array>
#include <cmath>
#include <vector>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetWindowName("MultipleGraphs");
  vtkNew<vtkRenderWindowInteractor> iRen;
  iRen->SetRenderWindow(renWin);

  // Setup the viewports
  auto gridDimensionsX = 2;
  auto gridDimensionsY = 1;
  auto rendererSizeX = 320;
  auto rendererSizeY = 240;
  renWin->SetSize(rendererSizeX * gridDimensionsX,
                  rendererSizeY * gridDimensionsY);
  std::cout << rendererSizeX * gridDimensionsX << " "
            << gridDimensionsY * gridDimensionsY << std::endl;
  std::vector<std::array<double, 4>> viewPorts;
  for (auto row = 0; row < gridDimensionsY; ++row)
  {
    for (auto col = 0; col < gridDimensionsX; ++col)
    {
      // index = row * grid_dimensions_x + col

      // (xmin, ymin, xmax, ymax)
      viewPorts.push_back(std::array<double, 4>{
          static_cast<double>(col) / gridDimensionsX,
          static_cast<double>(gridDimensionsY - (row + 1.0)) / gridDimensionsY,
          static_cast<double>(col + 1.0) / gridDimensionsX,
          static_cast<double>(gridDimensionsY - static_cast<double>(row)) /
              gridDimensionsY});
    }
  }

  // Link the renderers to the viewports.
  vtkNew<vtkRenderer> leftRenderer;
  leftRenderer->SetBackground(colors->GetColor3d("AliceBlue").GetData());
  leftRenderer->SetViewport(viewPorts[0].data());
  renWin->AddRenderer(leftRenderer);

  vtkNew<vtkRenderer> rightRenderer;
  rightRenderer->SetBackground(colors->GetColor3d("Lavender").GetData());
  rightRenderer->SetViewport(viewPorts[1].data());
  renWin->AddRenderer(rightRenderer);

  // Create the charts.
  vtkNew<vtkChartXY> leftChart;
  vtkNew<vtkContextScene> leftChartScene;
  vtkNew<vtkContextActor> leftChartActor;

  leftChartScene->AddItem(leftChart);
  leftChartActor->SetScene(leftChartScene);

  leftRenderer->AddActor(leftChartActor);
  leftChartScene->SetRenderer(leftRenderer);

  auto xAxis = leftChart->GetAxis(vtkAxis::BOTTOM);
  xAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightGrey"));
  xAxis->SetTitle("x");
  auto yAxis = leftChart->GetAxis(vtkAxis::LEFT);
  yAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightGrey"));
  yAxis->SetTitle("cos(x)");
  leftChart->GetBackgroundBrush()->SetColorF(
      colors->GetColor4d("MistyRose").GetData());
  leftChart->GetBackgroundBrush()->SetOpacityF(0.4);
  leftChart->SetTitle("Cosine");

  vtkNew<vtkChartXY> rightChart;
  vtkNew<vtkContextScene> rightChartScene;
  vtkNew<vtkContextActor> rightChartActor;

  rightChartScene->AddItem(rightChart);
  rightChartActor->SetScene(rightChartScene);

  rightRenderer->AddActor(rightChartActor);
  rightChartScene->SetRenderer(rightRenderer);

  xAxis = rightChart->GetAxis(vtkAxis::BOTTOM);
  xAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightCyan"));
  xAxis->SetTitle("x");
  yAxis = rightChart->GetAxis(vtkAxis::LEFT);
  yAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightCyan"));
  yAxis->SetTitle("sin(x)");
  rightChart->GetBackgroundBrush()->SetColorF(
      colors->GetColor4d("Thistle").GetData());
  rightChart->GetBackgroundBrush()->SetOpacityF(0.4);
  rightChart->SetTitle("Sine");

  // Create a table with some points in it.
  vtkNew<vtkTable> table;

  vtkNew<vtkFloatArray> arrX;
  arrX->SetName("X Axis");
  table->AddColumn(arrX);

  vtkNew<vtkFloatArray> arrC;
  arrC->SetName("Cosine");
  table->AddColumn(arrC);

  vtkNew<vtkFloatArray> arrS;
  arrS->SetName("Sine");
  table->AddColumn(arrS);

  //// Fill in the table with some example values.
  auto numPoints = 40;
  auto inc = 7.5 / (numPoints - 1.0);
  table->SetNumberOfRows(numPoints);
  for (int i = 0; i < numPoints; ++i)
  {
    table->SetValue(i, 0, i * inc);
    table->SetValue(i, 1, cos(i * inc));
    table->SetValue(i, 2, sin(i * inc));
  }

  auto ptColor = colors->GetColor4ub("Black");

  auto points = leftChart->AddPlot(vtkChart::POINTS);
  points->SetInputData(table, 0, 1);
  points->SetColor(ptColor.GetRed(), ptColor.GetGreen(), ptColor.GetBlue(),
                   ptColor.GetAlpha());
  points->SetWidth(1.0);
  dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::CROSS);

  points = rightChart->AddPlot(vtkChart::POINTS);
  points->SetInputData(table, 0, 2);
  points->SetColor(ptColor.GetRed(), ptColor.GetGreen(), ptColor.GetBlue(),
                   ptColor.GetAlpha());
  points->SetWidth(1.0);
  dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::PLUS);

  renWin->Render();
  iRen->Initialize();
  iRen->Start();

  return EXIT_SUCCESS;
}
