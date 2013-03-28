﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Utilities;

namespace CryEngine.Physics
{
    public class PhysicalEntityLiving : PhysicalEntity
    {
        internal PhysicalEntityLiving(IntPtr physEntPtr)
        {
            Handle = physEntPtr;
        }

        public override PhysicalizationType Type
        {
            get { return PhysicalizationType.Living; }
        }
    }

    public struct PlayerDynamics
    {
        public static PlayerDynamics Create()
        {
            var dyn = new PlayerDynamics();

            dyn.type = 4;

            dyn.kInertia = UnusedMarker.Float;
            dyn.kInertiaAccel = UnusedMarker.Float;
            dyn.kAirControl = UnusedMarker.Float;
            dyn.gravity = UnusedMarker.Vec3;
            dyn.nodSpeed = UnusedMarker.Float;
            dyn.mass = UnusedMarker.Float;
            dyn.bSwimming = UnusedMarker.Integer;
            dyn.surface_idx = UnusedMarker.Integer;
            dyn.bActive = UnusedMarker.Integer;
            dyn.collTypes = (EntityQueryFlags)UnusedMarker.Integer;
            dyn.livingEntToIgnore = UnusedMarker.IntPtr;
            dyn.minSlideAngle = UnusedMarker.Float;
            dyn.maxClimbAngle = UnusedMarker.Float;
            dyn.maxJumpAngle = UnusedMarker.Float;
            dyn.minFallAngle = UnusedMarker.Float;
            dyn.kAirResistance = UnusedMarker.Float;
            dyn.bNetwork = UnusedMarker.Integer;
            dyn.maxVelGround = UnusedMarker.Float;
            dyn.timeImpulseRecover = UnusedMarker.Float;
            dyn.iRequestedTime = UnusedMarker.Integer;

            return dyn;
        }

        internal int type;

        public float kInertia;    // inertia koefficient, the more it is, the less inertia is; 0 means no inertia
        public float kInertiaAccel; // inertia on acceleration
        public float kAirControl; // air control koefficient 0..1, 1 - special value (total control of movement)
        public float kAirResistance;    // standard air resistance 
        public Vec3 gravity; // gravity vector
        public float nodSpeed;    // vertical camera shake speed after landings
        public int bSwimming; // whether entity is swimming (is not bound to ground plane)
        public float mass;    // mass (in kg)
        public int surface_idx; // surface identifier for collisions
        public float minSlideAngle; // if surface slope is more than this angle, player starts sliding (angle is in radians)
        public float maxClimbAngle; // player cannot climb surface which slope is steeper than this angle
        public float maxJumpAngle; // player is not allowed to jump towards ground if this angle is exceeded
        public float minFallAngle;    // player starts falling when slope is steeper than this
        public float maxVelGround; // player cannot stand of surfaces that are moving faster than this
        public float timeImpulseRecover; // forcefully turns on inertia for that duration after receiving an impulse
        public EntityQueryFlags collTypes; // entity types to check collisions against
        IntPtr livingEntToIgnore;
        int bNetwork; // uses extended history information (obsolete)
        int bActive; // 0 disables all simulation for the character, apart from moving along the requested velocity
        int iRequestedTime; // requests that the player rolls back to that time and re-exucutes pending actions during the next step
    }

    public struct PlayerDimensions
    {
        internal static PlayerDimensions Create()
        {
            var dim = new PlayerDimensions();

            dim.type = 1;

            dim.dirUnproj = new Vec3(0, 0, 1);

            dim.sizeCollider = UnusedMarker.Vec3;
            dim.heightPivot = UnusedMarker.Float;
            dim.heightCollider = UnusedMarker.Float;
            dim.heightEye = UnusedMarker.Float;
            dim.heightHead = UnusedMarker.Float;
            dim.headRadius = UnusedMarker.Float;
            dim.bUseCapsule = UnusedMarker.Integer;

            return dim;
        }

        internal int type;

        public float heightPivot; // offset from central ground position that is considered entity center
        public float heightEye; // vertical offset of camera
        public Vec3 sizeCollider; // collision cylinder dimensions
        public float heightCollider;    // vertical offset of collision geometry center
        public float headRadius;    // radius of the 'head' geometry (used for camera offset)
        public float heightHead;    // center.z of the head geometry
        public Vec3 dirUnproj;    // unprojection direction to test in case the new position overlaps with the environment (can be 0 for 'auto')
        public float maxUnproj; // maximum allowed unprojection
        public int bUseCapsule; // switches between capsule and cylinder collider geometry
    }
}
