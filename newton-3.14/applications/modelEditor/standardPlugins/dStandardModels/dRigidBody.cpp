/////////////////////////////////////////////////////////////////////////////
// Name:        dRigidBody.cpp
// Purpose:     
// Author:      Julio Jerez
// Modified by: 
// Created:     22/05/2010 07:45:05
// RCS-ID:      
// Copyright:   Copyright (c) <2010> <Newton Game Dynamics>
// License:     
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "dRigidBody.h"



dRigidBody::dRigidBody()
	:dBodyPlugin()
{
}

dRigidBody::~dRigidBody()
{
}


dRigidBody* dRigidBody::GetPlugin()
{
	static dRigidBody plugin;
	return &plugin;
}


bool dRigidBody::Create (dPluginInterface* const interface)
{
	return true;
}


