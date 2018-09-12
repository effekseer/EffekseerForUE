% EffekseerForUnrealEngine4

<div class="main">

## Overview

This is Effekseer plugin for UnrealEngine 4.
With this plugin, you can play effects created with Effekseer on UnrealEngine 4.

### Environment

#### Version

UnrealEngine4.16 or later

#### Compiler

It is necessary to install a compiler suitable for the environment.
For example, you need to install VisualStudio 2015 or 2017 on Windows.

#### Platform

Since platform-specific functions are not used,
EffekseerForUnrealEngine4 works on most platforms.


## Constitution

EffekseerForUE4 consists mainly of two components.

It is ```EffekseerSystemComponent``` and ```EffekseerEmitterComponent```.

EffekseerSystem draws effects. Therefore, one component is absolutely necessary within the level.

EffekseerEmitterComponent creates and manipulates effects. It is necessary for each effect.

Besides these, theare are ```EffekseerEffect``` asset corresponding to Effekseer's efk file and
```EffekseerModel``` asset corresponding to the Effekseer's efkmdl file.

There are also ```BP_EffekseerSystem``` and ```BP_EffekseerEmitter``` blueprints that makes it easier to use components.

## Sample

``` EffekseerForUE4.uproject ``` contains sample levels.

You can see that Effekseer's effect is being played.


<div align="center">
<img src="img/ss.png">
</div>


## Installation and usage

This section explains how to install and use EffekseerForUE 4 in your project.

### 1.1. Copying directories

You copy the ```Plugins``` directory to the same directory as the uproject you created.

<div align="center">
<img src="img/copyFiles.png">
</div>

### 1.2. Adding C ++ code

You add some C ++ code to the project. It is no problem even if contents are empty.

<div align="center">
<img src="img/newCpp.png">
</div>

<div align="center">
<img src="img/classNone.png">
</div>


### 1.3. Enabling Plugin

You activate Plugin from UnrealEngine4. If it may be required to restart, you restart UnrealEngine 4.

<div align="center">
<img src="img/enabledPlugin.png">
</div>

You may be asked to compile after rebooting. You compile the project.

<div align="center">
<img src="img/recompile.png">
</div>

### 1.4. Adding a blueprint

You add ```BP_EffekseerSystem``` and ```BP_EffekseerEmitter``` to the level.

Since these are in the contents of the plugin, you make the contents of the plugin shown.

<div align="center">
<img src="img/showPluginContent.png">
</div>

You move to the directory of the contents of the plugin.


<div align="center">
<img src="img/moveDirectory.png">
</div>

You add blueprints to the level.

<div align="center">
<img src="img/blueprints.png">
</div>

Currently, BP_EffekseerSystem must be located at position (0, 0, 0).

<div align="center">
<img src="img/effekseerSystem.png">
</div>

BP_EffekseerEmitter be located at the position where you want to display an effect.

<div align="center">
<img src="img/effekseerEmitter.png">
</div>

### 1.5. Adding assets

You add the efk files and textures output from Effekseer to the content.

<div align="center">
<img src="img/addAssets.png">
</div>

You assigns textures to the EffekseerEffect asset.
Right-click the EffekseerEffect asset, and then run AssignResources.

<div align="center">
<img src="img/assignResource.png">
</div>

Since the effect is often small, double-click the EffekseerEffect asset to set the scale.
Let's enter a large number to make the effect easier to see.

<div align="center">
<img src="img/changeScale.png">
</div>

You select BP_EffekseerEmitter to display the component.
You set the EffekseerEffect asset you just read into the EffekseerEmitter component.

<div align="center">
<img src="img/setEffect.png">
</div>

You turn on Auto Activate of the EffekseerEmitter component.

<div align="center">
<img src="img/activate.png">
</div>

If you play it, you will see that the effect is displayed.

<div align="center">
<img src="img/play.png">
</div>

### 2.1. Components of Effekseer

You can also add Effekseer components to other actors.
However, since EffekseerSystem is complicated, we recommend that you only use the EffekseerEmitter component.

<div align="center">
<img src="img/addComponent.png">
</div>

### 3.1. Animation Notifications

Effects can be played using Animation Notifications.
Effects can be played at indicated time in Animation Sequence.

<div align="center">
<img src="img/animNotify.png">
</div>


## Caution

A greyscale image with alpha will display incorrectly.

## Limitation

Subtraction, and distortion are different from Effekseer.

The normal map to the model does not apply.

If a large number of models are drawn, the speed tends to decrease.

## Todo

- Match of display of Effekseer and UnrealEngine4 of subtraction, distortion

- Implementation of model normals

- Improved ease of use

## Release note

### 1.40 (2018/09/12) 

- Supported 4.20

- Add functions(Playback speed and color settings)

### 1.31 (2017/11/20) 

- Supported Animation Notifications

- Fixed many bugs

### 1.30 (2017/11/01)

- First release

## Lisence

<pre>

The MIT License (MIT)

Copyright (c) 2011 Effekseer Project

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</pre>

</div>



