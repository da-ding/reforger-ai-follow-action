class DAD_FollowAction : ScriptedUserAction {

	// TODO: It might be bad keeping a waypoint around for every Character that is FollowAction'ed
	SCR_EntityWaypoint m_FollowWaypoint = null;
	
	bool m_fixedSpeedBug = false;

	const ResourceName m_WaypointType = "{C37ABB3DCAE43B36}Prefabs/AI/Waypoints/AIWaypoint_FollowFast.et";

	
	IEntity m_User = null;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent rplC = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rplC.IsOwner()) return;

		if (!m_FollowWaypoint)
		{
			Resource wpRes = Resource.Load(m_WaypointType);
			m_FollowWaypoint = SCR_EntityWaypoint.Cast(SpawnHelpers.SpawnEntity(wpRes, pUserEntity.GetOrigin()));
		}

		AIControlComponent controlComp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		AIGroup ai = controlComp.GetControlAIAgent().GetParentGroup();

		if (IsFollowing())
		{
			ai.RemoveWaypointAt(0);
			m_User = null;
		}
		else
		{
			m_User = pUserEntity;
			m_FollowWaypoint.SetEntity(m_User);
			
			ai.AddWaypointAt(m_FollowWaypoint, 0);
			UpdateWaypointPos();
		}

		AIWaypoint wp = ai.GetCurrentWaypoint();

		bool isFollowing;
		if (!wp)
		{
			isFollowing = false;
		}
		else
		{
			EntityPrefabData prefab = wp.GetPrefabData();
			if (!prefab) Print("Could not get prefab!");
			isFollowing = prefab && prefab.GetPrefabName().Contains("Waypoint_Follow");
		}

		array<AIAgent> agents = new array<AIAgent>();
		ai.GetAgents(agents);
		foreach (AIAgent agent : agents)
		{
			SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
			char.SetIsFollowing(isFollowing);

		}
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (IsFollowing())
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
		AIGroup ai = SCR_AIGroup.Cast(controlComp.GetControlAIAgent().GetParentGroup());

		if (!ai)
		{
			Print("DAD_FollowAction: Could not find AI Group", LogLevel.ERROR);
			return null;
		}
		return ai;
	}

	//[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool IsFollowing()
	{
		RplComponent rplC = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());
		return owner.GetIsFollowing();
	}
	
	void UpdateWaypointPos() {
		if (!m_User) return;
		if (m_fixedSpeedBug) return;
		
		GetGame().GetCallqueue().CallLater(UpdateWaypointPos, 4 * 1000, false);
		
		vector newOrigin = m_User.GetOrigin();		
		float distance = vector.Distance(GetOwner().GetOrigin(), newOrigin);

		if (distance < 5) return;
		
		AIGroup ai = GetAI();

		AIWaypoint moveWaypoint = AIWaypoint.Cast(SpawnHelpers.SpawnEntity(
				Resource.Load("{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et"),
				newOrigin
		));

		moveWaypoint.SetOrigin(newOrigin);
		ai.AddWaypointAt(moveWaypoint, 0);
		m_fixedSpeedBug = true;
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
