#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCellLocator.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetEventTranslator.h>

namespace {

class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback* New()
  {
    return new vtkSliderCallback;
  }
  vtkSliderCallback() : CellLocator(0), Level(0), PolyData(0), Renderer(0)
  {
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    this->Level = vtkMath::Round(
        static_cast<vtkSliderRepresentation*>(sliderWidget->GetRepresentation())
            ->GetValue());
    this->CellLocator->GenerateRepresentation(this->Level, this->PolyData);
    this->Renderer->Render();
  }

  vtkCellLocator* CellLocator;
  int Level;
  vtkPolyData* PolyData;
  vtkRenderer* Renderer;
};

void SetSliderColors(vtkSliderRepresentation2D* slider);

} // namespace
int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> inputSource;
  inputSource->SetPhiResolution(10);
  inputSource->SetThetaResolution(10);
  inputSource->Update();

  vtkNew<vtkPolyDataMapper> pointsMapper;
  pointsMapper->SetInputConnection(inputSource->GetOutputPort());

  vtkNew<vtkActor> pointsActor;
  pointsActor->SetMapper(pointsMapper);
  pointsActor->GetProperty()->SetInterpolationToFlat();
  pointsActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Create the tree
  vtkNew<vtkCellLocator> cellLocator;
  cellLocator->SetDataSet(inputSource->GetOutput());
  cellLocator->BuildLocator();

  // Initialize the representation
  vtkNew<vtkPolyData> polydata;
  cellLocator->GenerateRepresentation(0, polydata);

  vtkNew<vtkPolyDataMapper> locatorTreeMapper;
  locatorTreeMapper->SetInputData(polydata);

  vtkNew<vtkActor> locatorTreeActor;
  locatorTreeActor->SetMapper(locatorTreeMapper);
  locatorTreeActor->GetProperty()->SetInterpolationToFlat();
  locatorTreeActor->GetProperty()->SetRepresentationToWireframe();
  locatorTreeActor->GetProperty()->SetColor(
      colors->GetColor3d("Gold").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CellLocatorVisualization");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(pointsActor);
  renderer->AddActor(locatorTreeActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render an image (lights and cameras are created automatically)
  renderWindow->Render();

  vtkNew<vtkSliderRepresentation2D> sliderRep;
  sliderRep->SetMinimumValue(0);
  sliderRep->SetMaximumValue(cellLocator->GetLevel());
  sliderRep->SetValue(0);
  sliderRep->SetTitleText("MaxPointsPerRegion");
  sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  sliderRep->GetPoint1Coordinate()->SetValue(.2, .1);
  sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  sliderRep->GetPoint2Coordinate()->SetValue(.8, .1);
  sliderRep->SetSliderLength(0.075);
  sliderRep->SetSliderWidth(0.05);
  sliderRep->SetEndCapLength(0.05);
  SetSliderColors(sliderRep);

  vtkNew<vtkSliderWidget> sliderWidget;
  sliderWidget->SetInteractor(renderWindowInteractor);
  sliderWidget->SetRepresentation(sliderRep);
  sliderWidget->SetAnimationModeToAnimate();
  sliderWidget->EnabledOn();

  vtkNew<vtkSliderCallback> callback;
  callback->CellLocator = cellLocator;
  callback->PolyData = polydata;
  callback->Renderer = renderer;

  sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

  renderWindowInteractor->Initialize();
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

void SetSliderColors(vtkSliderRepresentation2D* slider)
{
  vtkNew<vtkNamedColors> colors;
  // Set color properties:
  // Change the color of the knob that slides
  slider->GetSliderProperty()->SetColor(colors->GetColor3d("Peru").GetData());
  // Change the color of the text indicating what the slider controls
  slider->GetTitleProperty()->SetColor(colors->GetColor3d("Silver").GetData());
  // Change the color of the text displaying the value
  slider->GetLabelProperty()->SetColor(colors->GetColor3d("Silver").GetData());
  // Change the color of the knob when the mouse is held on it
  slider->GetSelectedProperty()->SetColor(
      colors->GetColor3d("DeepPink").GetData());
  // Change the color of the bar
  slider->GetTubeProperty()->SetColor(colors->GetColor3d("Teal").GetData());
  // Change the color of the ends of the bar
  slider->GetCapProperty()->SetColor(colors->GetColor3d("Teal").GetData());
}

} // namespace
