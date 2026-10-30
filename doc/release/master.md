YARP <yarp-4.1.0> (UNRELEASED)                                         {#yarp_4_1_0}
============================

[TOC]

YARP <yarp-4.1.0> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-4.1.0%22).

### libYARP_profiler

* Added tests for CI

### libYARP_os

* class `yarp::os::Property` is no more used for any network serialization routine.

### libYARP_dev

* added new thrift data types `DistortionModelData`, `IntrinsicParamsData` to replace the use of `yarp::os::Property` in `IRgbVisualParams.h`, `IDepthVisualParams.h` interfaces.
* Several methods of `MultipleAnalogSensor` interfaces migrated from returning `bool` to returning `yarp::dev::ReturnValue`
* New method for `ISpeechSynthesizer`: `getAvailableVoices` to get a list of the available voices names for the current language

### libYARP_manager

* Added application parameters: an application can use `$PAR{name}` or `$PAR{name=default}` placeholders and the applications
  embedding it can set their values with `<app-param name="name">value</app-param>`, so the same application can be embedded
  several times with different values. See the yarpmanager documentation for details.
* Fixed `Application::clear()` not clearing the arbitrators, which leaked from one application file to the next when loading a directory.

### yarpmanager

* The builder warns before saving an application that uses application parameters, and shows an error when an embedded
  application cannot be added.

### thrift generator

* Updated thrift generator to use `yarp.api.include` also in `enums` when required.
* Updated thrift generator to add an error message if the conversion of an enum toString() or fromString() fails.
* Fixed code generation for nested map values: each container depth now uses its own map value name, so structures containing maps of maps or maps of lists of maps compile correctly.

### devices

* `ControlBoardRemapper` and `RemoteControlBoardRemapper` refactored to support ParamParser.
* `MultipleAnalogSensorClient` and `MultipleAnalogSensorServer` now use the new `IMultipleAnalogSensor` interfaces returning `yarp::dev::ReturnValue`
* Updated `ISpeechSynthesizerMsgs.thrift` and correspondent generated code for the new method `getAvailableVoices`
* Updated devices related to `yarp::dev::ISpeechSynthesizer` for the new method `getAvailableVoices`:
  * `FakeSpeechSynthesizer`
  * `SpeechSynthesizer_nws_yarp`
  * `SpeechSynthesizer_nwc_yarp`

### bindings

* `LogComponent` added to bindings to improve yarp logging system integration
* `IRGBDSensor` Added missing include and extensions to fully use the IRGBDSensor interface with the bindings.