/*
    Extended vehicle functions by Emmet_

	Thanks to:

	- [uL]Pottus -> GetVehicleTopSpeed idea
	- kvann -> SetVehicleInterior idea
	- Hanger -> Top speed data
*/

#if !defined INVALID_SEAT_ID
	#define INVALID_SEAT_ID 		-1
#endif

#if !defined INVALID_PAINTJOB_ID
	#define INVALID_PAINTJOB_ID     -1
#endif

#define GetVehicleSeats(%0) \
	(GetVehicleModelSeats(GetVehicleModel(%0)))

#define SetVehicleInterior(%0) \
	(LinkVehicleToInterior(%0))

#define GetVehicleBoot(%0,%1,%2,%3) \
	(GetVehicleOffset((%0), VEHICLE_OFFSET_BOOT, %1, %2, %3))

#define GetVehicleHood(%0,%1,%2,%3) \
	(GetVehicleOffset((%0), VEHICLE_OFFSET_HOOD, %1, %2, %3))

#define GetVehicleRoof(%0,%1,%2,%3) \
	(GetVehicleOffset((%0), VEHICLE_OFFSET_ROOF, %1, %2, %3))

enum e_VehicleProperties {
	e_VEHICLE_INTERIOR,
	e_VEHICLE_PAINTJOB,
	e_VEHICLE_COLOR_1,
	e_VEHICLE_COLOR_2
};

static s_VehicleTrailerID[MAX_VEHICLES];

// Called when a vehicle is created.
forward OnVehicleCreated(vehicleid);
// Called when a vehicle is deleted.
forward OnVehicleDestroyed(vehicleid);
// Called when a trailer is hooked.
forward OnTrailerHooked(playerid, vehicleid, trailerid);
// Called when a trailer is unhooked.
forward OnTrailerUnhooked(playerid, vehicleid, trailerid);
// Called when a player shoots at a vehicle.
forward OnPlayerShootVehicle(playerid, vehicleid, weaponid);

enum e_ParamTypes {
	VEHICLE_TYPE_ENGINE,
	VEHICLE_TYPE_LIGHTS,
 	VEHICLE_TYPE_ALARM,
	VEHICLE_TYPE_DOORS,
	VEHICLE_TYPE_BONNET,
	VEHICLE_TYPE_BOOT,
	VEHICLE_TYPE_OBJECTIVE
};

enum e_OffsetTypes {
	VEHICLE_OFFSET_BOOT,
	VEHICLE_OFFSET_HOOD,
	VEHICLE_OFFSET_ROOF
};

#if !defined IsValidVehicle
	native IsValidVehicle(vehicleid);
#endif

static const s_TopSpeed[212] = {
	157, 147, 186, 110, 133, 164, 110, 148, 100, 158, 129, 221, 168, 110, 105, 192, 154, 270,
	115, 149, 145, 154, 140, 99, 135, 270, 173, 165, 157, 201, 190, 130, 94, 110, 167, 0, 149,
	158, 142, 168, 136, 145, 139, 126, 110, 164, 270, 270, 111, 0, 0, 193, 270, 60, 135, 157,
	106, 95, 157, 136, 270, 160, 111, 142, 145, 145, 147, 140, 144, 270, 157, 110, 190, 190,
	149, 173, 270, 186, 117, 140, 184, 73, 156, 122, 190, 99, 64, 270, 270, 139, 157, 149, 140,
	270, 214, 176, 162, 270, 108, 123, 140, 145, 216, 216, 173, 140, 179, 166, 108, 79, 101, 270,
    270, 270, 120, 142, 157, 157, 164, 270, 270, 160, 176, 151, 130, 160, 158, 149, 176, 149, 60,
    70, 110, 167, 168, 158, 173, 0, 0, 270, 149, 203, 164, 151, 150, 147, 149, 142, 270, 153, 145,
    157, 121, 270, 144, 158, 113, 113, 156, 178, 169, 154, 178, 270, 145, 165, 160, 173, 146, 0, 0,
    93, 60, 110, 60, 158, 158, 270, 130, 158, 153, 151, 136, 85, 0, 153, 142, 165, 108, 162, 0, 0,
    270, 270, 130, 190, 175, 175, 175, 158, 151, 110, 169, 171, 148, 152, 0, 0, 0, 108, 0, 0
};

/*
    native GetVehicleParams(vehicleid, type);
	native SetVehicleParams(vehicleid, type, status, delay = 0);
	native GetVehicleBoot(vehicleid, &Float:x, &Float:y, &Float:z);
	native GetVehicleHood(vehicleid, &Float:x, &Float:y, &Float:z);
	native GetVehicleRoof(vehicleid, &Float:x, &Float:y, &Float:z);
	native GetVehicleRandomColors(modelid, &color1, &color2);
	native GetVehicleColor(vehicleid, &color1, &color2);
	native GetNearestVehicle(vehicleid);
	native GetVehiclePaintjob(vehicleid);
	native GetVehicleDriver(vehicleid);
	native GetVehicleTopSpeed(vehicleid);
	native GetVehicleInterior(vehicleid);
	native GetVehicleSeats(vehicleid);
	native GetVehicleModelSeats(modelid);
	native ResetVehiclePaintjob(vehicleid);
	native IsVehicleSeatOccupied(vehicleid, seatid);
	native IsValidVehicle(vehicleid);
*/

forward EVF_SetVehicleParamsEx(vehicleid, bool:engine, bool:lights, bool:alarm, bool:doors, bool:bonnet, bool:boot, bool:objective);
public EVF_SetVehicleParamsEx(vehicleid, bool:engine, bool:lights, bool:alarm, bool:doors, bool:bonnet, bool:boot, bool:objective)
{
	SetVehicleParamsEx(vehicleid, bool:engine, bool:lights, bool:alarm, bool:doors, bool:bonnet, bool:boot, bool:objective);
}

stock static ResetVehicleProperties(vehicleid)
{
    s_VehicleTrailerID[vehicleid] = INVALID_VEHICLE_ID;

	for (new i = 0; i < _:e_VehicleProperties; i ++)
	{
	    deleteproperty(4, !"", (vehicleid << 2) + i);
	}
}

stock static SetVehiclePropertyValue(vehicleid, type, value)
{
	new
	    string[12];

	format(string, sizeof(string), "%d", value);

	setproperty(4, !"", ((vehicleid << 2) + type), string);
}

stock static GetVehiclePropertyValue(vehicleid, type)
{
	new
	    string[12];

	getproperty(4, !"", ((vehicleid << 2) + type), string);

	return strval(string);
}

stock static IsVehiclePropertySet(vehicleid, type)
{
    return existproperty(4, !"", (vehicleid << 2) + type);
}

stock GetVehicleParams(vehicleid, type)
{
	new
		params[7]
	;
	GetVehicleParamsEx(vehicleid, params[0], params[1], params[2], params[3], params[4], params[5], params[6]);

	return (!(params[type] <= 0));
}

stock SetVehicleParams(vehicleid, type, status, delay = 0)
{
	new
		params[7];

    if (GetVehicleParamsEx(vehicleid, bool:params[0], bool:params[1], bool:params[2], bool:params[3], bool:params[4], bool:params[5], bool:params[6]))
	{
		params[type] = status;

		if (delay > 0)
		{
			SetTimerEx("EVF_SetVehicleParamsEx", delay, false, "iiiiiiii", vehicleid, params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
		}
		else
		{
			SetVehicleParamsEx(vehicleid, bool:params[0], bool:params[1], bool:params[2], bool:params[3], bool:params[4], bool:params[5], bool:params[6]);
		}
		return 1;
	}
	return 0;
}

stock GetVehicleOffset(vehicleid, type, &Float:x, &Float:y, &Float:z)
{
    new Float:fPos[4], Float:fSize[3];

	if (!IsValidVehicle(vehicleid))
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;

		return 0;
	}
	else
	{
		GetVehiclePos(vehicleid, fPos[0], fPos[1], fPos[2]);
		GetVehicleZAngle(vehicleid, fPos[3]);
		GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, fSize[0], fSize[1], fSize[2]);

		switch (type)
		{
		    case VEHICLE_OFFSET_BOOT:
		    {
				x = fPos[0] - (floatsqroot(fSize[1] + fSize[1]) * floatsin(-fPos[3], degrees));
				y = fPos[1] - (floatsqroot(fSize[1] + fSize[1]) * floatcos(-fPos[3], degrees));
 				z = fPos[2];
			}
			case VEHICLE_OFFSET_HOOD:
			{
			    x = fPos[0] + (floatsqroot(fSize[1] + fSize[1]) * floatsin(-fPos[3], degrees));
				y = fPos[1] + (floatsqroot(fSize[1] + fSize[1]) * floatcos(-fPos[3], degrees));
	 			z = fPos[2];
			}
			case VEHICLE_OFFSET_ROOF:
			{
			    x = fPos[0];
			    y = fPos[1];
			    z = fPos[2] + floatsqroot(fSize[2]);
			}
		}
	}
	return 1;
}

stock GetNearestVehicle(playerid)
{
 	new
	 	Float:fX,
	 	Float:fY,
	 	Float:fZ,
	 	Float:fSX,
	    Float:fSY,
		Float:fSZ,
		Float:fRadius;

	for (new i = 1, j = GetVehiclePoolSize(); i <= j; i ++)
	{
	    if (!IsVehicleStreamedIn(i, playerid))
		{
			continue;
	    }
	    else
	    {
			GetVehiclePos(i, fX, fY, fZ);

			GetVehicleModelInfo(GetVehicleModel(i), VEHICLE_MODEL_INFO_SIZE, fSX, fSY, fSZ);

			fRadius = floatsqroot((fSX + fSX) + (fSY + fSY));

			if (IsPlayerInRangeOfPoint(playerid, fRadius, fX, fY, fZ) && GetPlayerInterior(playerid) == GetVehicleInterior(i) && GetPlayerVirtualWorld(playerid) == GetVehicleVirtualWorld(i))
			{
				return i;
			}
		}
	}
	return INVALID_VEHICLE_ID;
}

stock IsVehicleSeatOccupied(vehicleid, seatid)
{
	if (!GetVehicleModel(vehicleid))
	{
		return 0;
	}
	else
	{
		for (new i = 0; i < MAX_PLAYERS; i ++)
		{
	    	if (IsPlayerInVehicle(i, vehicleid) && GetPlayerVehicleSeat(i) == seatid)
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsVehicleFull(vehicleid)
{
    for (new i = 0; i < 4; i++)
    {
        if (IsVehicleSeatOccupied(vehicleid, i))
        {
            return false;
        }
    }
    return true;
}

stock GetVehicleNextSeat(vehicleid, passenger = 1)
{
	new
	    seats = GetVehicleSeats(vehicleid);

	if (seats > 1)
	{
	    for (new i = passenger; i < seats; i ++)
		{
		    if (!IsVehicleSeatOccupied(vehicleid, i))
			{
				return i;
			}
		}
	}
	return INVALID_SEAT_ID;
}

stock GetVehicleModelSeats(modelid)
{
    static const s_VehicleSeats[] =
	{
		4, 2, 2, 2, 4, 4, 1, 2, 2, 4, 2, 2, 2, 4, 2, 2, 4, 2, 4, 2, 4, 4, 2, 2, 2, 1, 4, 4, 4, 2,
		1, 7, 1, 2, 2, 0, 2, 7, 4, 2, 4, 1, 2, 2, 2, 4, 1, 2, 1, 0, 0, 2, 1, 1, 1, 2, 2, 2, 4,
		4, 2, 2, 2, 2, 1, 1, 4, 4, 2, 2, 4, 2, 1, 1, 2, 2, 1, 2, 2, 4, 2, 1, 4, 3, 1, 1, 1, 4, 2,
		2, 4, 2, 4, 1, 2, 2, 2, 4, 4, 2, 2, 1, 2, 2, 2, 2, 2, 4, 2, 1, 1, 2, 1, 1, 2, 2, 4, 2, 2,
		1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 4, 1, 1, 1, 2, 2, 2, 2, 7, 7, 1, 4, 2, 2, 2, 2, 2, 4, 4,
		2, 2, 4, 4, 2, 1, 2, 2, 2, 2, 2, 2, 4, 4, 2, 2, 1, 2, 4, 4, 1, 0, 0, 1, 1, 2, 1, 2, 2, 1, 2,
		4, 4, 2, 4, 1, 0, 4, 2, 2, 2, 2, 0, 0, 7, 2, 2, 1, 4, 4, 4, 2, 2, 2, 2, 2, 4, 2, 0, 0, 0,
		4, 0, 0
	};
	return (modelid < 400 || modelid > 611) ? (0) : (s_VehicleSeats[(modelid - 400)]);
}

stock Float:GetVehicleTopSpeed(vehicleid)
{
	new model = GetVehicleModel(vehicleid);

	if (model)
 	{
  		return float(s_TopSpeed[(model - 400)]);
    }
   	return 0.0;
}

stock ResetVehiclePaintjob(vehicleid)
{
	if (IsValidVehicle(vehicleid))
	{
	    ChangeVehiclePaintjob(vehicleid, INVALID_PAINTJOB_ID);
	    SetVehiclePropertyValue(vehicleid, e_VEHICLE_PAINTJOB, INVALID_PAINTJOB_ID);
	    return 1;
	}
	return 0;
}

stock EVF_LinkVehicleToInterior(vehicleid, interiorid)
{
	new
	    ret = LinkVehicleToInterior(vehicleid, interiorid);

	if (ret)
	{
		SetVehiclePropertyValue(vehicleid, e_VEHICLE_INTERIOR, interiorid);
	}
	return ret;
}

stock EVF_DestroyVehicle(vehicleid)
{
    if (IsValidVehicle(vehicleid))
    {
        if (CallLocalFunction("OnVehicleDestroyed", "i", vehicleid) > 0 || funcidx("OnVehicleDestroyed") == -1)
        {
            ResetVehicleProperties(vehicleid);

            return DestroyVehicle(vehicleid);
        }
    }
    return 0;
}
#if defined _ALS_DestroyVehicle
    #undef DestroyVehicle
#else
    #define _ALS_DestroyVehicle
#endif
 
#define DestroyVehicle EVF_DestroyVehicle

stock EVF_AddStaticVehicle(modelid, Float:x, Float:y, Float:z, Float:a, color1, color2)
{
    new vehicleid = INVALID_VEHICLE_ID;

    if (400 <= modelid <= 611)
    {
        vehicleid = AddStaticVehicle(modelid, x, y, z, a, color1, color2);

        if (vehicleid != INVALID_VEHICLE_ID)
        {
            ResetVehicleProperties(vehicleid);

            SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_1, color1);
            SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_2, color2);

            CallLocalFunction("OnVehicleCreated", "i", vehicleid);
        }
    }
    return vehicleid;
}
#if defined _ALS_AddStaticVehicle
    #undef AddStaticVehicle
#else
    #define _ALS_AddStaticVehicle
#endif
 
#define AddStaticVehicle EVF_AddStaticVehicle

stock EVF_AddStaticVehicleEx(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, color1, color2, respawn_delay)
{
    new vehicleid = INVALID_VEHICLE_ID;

    if (400 <= modelid <= 611)
    {
        vehicleid = AddStaticVehicleEx(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, color1, color2, respawn_delay);

        if (vehicleid != INVALID_VEHICLE_ID)
        {
            ResetVehicleProperties(vehicleid);

            SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_1, color1);
            SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_2, color2);

            CallLocalFunction("OnVehicleCreated", "i", vehicleid);
        }
    }
    return vehicleid;
}
#if defined _ALS_AddStaticVehicle
    #undef AddStaticVehicle
#else
    #define _ALS_AddStaticVehicle
#endif
 
#define AddStaticVehicle EVF_AddStaticVehicle

stock EVF_ChangeVehicleColor(vehicleid, color1, color2)
{
	new
	    ret = ChangeVehicleColor(vehicleid, color1, color2);

	if (ret)
	{
	    SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_1, color1);
	    SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_2, color2);
	}
	return ret;
}

stock EVF_ChangeVehiclePaintjob(vehicleid, paintjobid)
{
	new
	    ret = ChangeVehiclePaintjob(vehicleid, paintjobid);

	if (ret)
	{
		SetVehiclePropertyValue(vehicleid, e_VEHICLE_PAINTJOB, paintjobid);
	}
	return ret;
}

public OnPlayerUpdate(playerid)
{
	if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
	    new
			vehicleid = GetPlayerVehicleID(playerid),
			trailerid = GetVehicleTrailer(vehicleid)
		;

		if (s_VehicleTrailerID[vehicleid] != trailerid)
	    {
	        if (trailerid != 0)
			{
				CallLocalFunction("OnTrailerHooked", "iii", playerid, vehicleid, trailerid);
			}
			else
			{
			    CallLocalFunction("OnTrailerUnhooked", "iii", playerid, vehicleid, s_VehicleTrailerID[vehicleid]);
			}
			s_VehicleTrailerID[vehicleid] = trailerid;
		}
	}
	#if defined EVF_OnPlayerUpdate
	    return EVF_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerWeaponShot(playerid, WEAPON:weaponid, BULLET_HIT_TYPE:hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if ((BULLET_HIT_TYPE:hittype == BULLET_HIT_TYPE_VEHICLE) && (WEAPON:22 <= weaponid <= WEAPON:38))
	{
	    CallLocalFunction("OnPlayerShootVehicle", "iii", playerid, hitid, weaponid);
	}
	#if defined EVF_OnPlayerWeaponShot
	    return EVF_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, fX, fY, fZ);
	#else
	    return 1;
	#endif
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
    SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_1, color1);
    SetVehiclePropertyValue(vehicleid, e_VEHICLE_COLOR_2, color2);

	#if defined EVF_OnVehicleRespray
	    return EVF_OnVehicleRespray(playerid, vehicleid, color1, color2);
	#else
	    return 1;
	#endif
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
	SetVehiclePropertyValue(vehicleid, e_VEHICLE_PAINTJOB, paintjobid);

	#if defined EVF_OnVehiclePaintjob
	    return EVF_OnVehiclePaintjob(playerid, vehicleid, paintjobid);
	#else
	    return 1;
	#endif
}

#if defined _ALS_LinkVehicleToInterior
	#define LinkVehicleToInterior
#else
	#define _ALS_LinkVehicleToInterior
#endif

#if defined _ALS_ChangeVehicleColor
	#define ChangeVehicleColor
#else
	#define _ALS_ChangeVehicleColor
#endif

#if defined _ALS_ChangeVehiclePaintjob
	#define ChangeVehiclePaintjob
#else
	#define _ALS_ChangeVehiclePaintjob
#endif

#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif

#if defined _ALS_OnVehicleRespray
	#undef OnVehicleRespray
#else
	#define _ALS_OnVehicleRespray
#endif

#if defined _ALS_OnVehiclePaintjob
	#undef OnVehiclePaintjob
#else
	#define _ALS_OnVehiclePaintjob
#endif

#define LinkVehicleToInterior 	EVF_LinkVehicleToInterior
#define ChangeVehicleColor      EVF_ChangeVehicleColor
#define ChangeVehiclePaintjob	EVF_ChangeVehiclePaintjob
#define OnPlayerUpdate          EVF_OnPlayerUpdate
#define OnPlayerWeaponShot      EVF_OnPlayerWeaponShot
#define OnVehicleRespray        EVF_OnVehicleRespray
#define OnVehiclePaintjob       EVF_OnVehiclePaintjob

#if defined EVF_OnPlayerUpdate
	forward EVF_OnPlayerUpdate(playerid);
#endif

#if defined EVF_OnPlayerWeaponShot
	forward EVF_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

#if defined EVF_OnVehicleRespray
	forward EVF_OnVehicleRespray(playerid, vehicleid, color1, color2);
#endif

#if defined EVF_OnVehiclePaintjob
	forward EVF_OnVehiclePaintjob(playerid, vehicleid, paintjobid);
#endif