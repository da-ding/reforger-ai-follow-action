class DAD_FollowAction : ScriptedUserAction {

	// TODO: It might be bad keeping a waypoint around for every Character that is FollowAction'ed
	SCR_EntityWaypoint m_FollowWaypoint = null;

	const ResourceName m_WaypointType = "{C37ABB3DCAE43B36}Prefabs/AI/Waypoints/AIWaypoint_FollowFast.et";

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent rplC = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rplC.IsOwner()) return;

		// TODO: We have to update all of the Group's children here with the isFollowing status, because GetActionNameScript
		// only runs on the proxy/client/whatever
		// Also, we probably want to do that automatically in the AIGroup whenever a Follow waypoint is set.

		if (!m_FollowWaypoint)
		{
			Resource wpRes = Resource.Load(m_WaypointType);
			m_FollowWaypoint = SCR_EntityWaypoint.Cast(SpawnHelpers.SpawnEntity(wpRes, pUserEntity.GetOrigin()));
		}

		AIControlComponent controlComp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		AIGroup ai = controlComp.GetControlAIAgent().GetParentGroup();

		if (CheckIsFollowing())
		{
			ai.RemoveWaypointAt(0);
		}
		else
		{
			m_FollowWaypoint.SetEntity(pUserEntity);
			//m_FollowWaypoint.SetPriority(true);

			ai.AddWaypointAt(m_FollowWaypoint, 0);
		}
		CheckIsFollowing();
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (CheckIsFollowing())
			outName = "Stop from Following";
		else
			outName = "Ask to Follow";

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(user);
		if (!owner.GetFaction().IsFactionFriendly(player.GetFaction())) return false;

		//AIControlComponent aiComp = AIControlComponent.Cast(owner.FindComponent(AIControlComponent));
		//if (!aiComp.IsAIActivated()) return false;

		return true;
	}

	override bool CanBroadcastScript()
	{
		return true;
	}

	AIGroup GetAI()
	{
		AIControlComponent controlComp = AIControlComponent.Cast(GetOwner().FindComponent(AIControlComponent));
		AIGroup ai = controlComp.GetControlAIAgent().GetParentGroup();

		if (!ai)
		{
			Print("DAD_FollowAction: Could not find AI Group", LogLevel.ERROR);
			return null;
		}
		return ai;
	}

	//[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool CheckIsFollowing()
	{
		RplComponent rplC = RplComponent.Cast(GetOwner().FindComponent(RplComponent));

		if (!rplC.IsOwner())
		{
			SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());
			return owner.GetIsFollowing();
		}

		AIGroup ai = GetAI();
		AIWaypoint wp = ai.GetCurrentWaypoint();

		bool isFollowing;
		if (!wp)
		{
			isFollowing = false;
		}
		else
		{
			EntityPrefabData prefab = wp.GetPrefabData();
			isFollowing = prefab && prefab.GetPrefabName().Contains("Waypoint_Follow");
		}

		if (!ai) return false;
		array<AIAgent> agents = new array<AIAgent>();
		ai.GetAgents(agents);
		foreach (AIAgent agent : agents)
		{
			SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
			char.SetIsFollowing(isFollowing);
		}
		return isFollowing;
	}

	/*
	//! Before performing the action the caller can store some data in it which is delivered to others.
	//! Only available for actions for which HasLocalEffectOnly returns false.
	override event protected bool OnSaveActionData(ScriptBitWriter writer)
	{
		CheckIsFollowing();
		writer.WriteBool(!m_IsFollowing);
		return true;
	}
	//! If the one performing the action packed some data in it everybody receiving the action.
	//! Only available for actions for which HasLocalEffectOnly returns false.
	//! Only triggered if the sender wrote anyting to the buffer.
	override event protected bool OnLoadActionData(ScriptBitReader reader)
	{
		reader.ReadBool(m_IsFollowing);
		return true;
	}
	*/
}
