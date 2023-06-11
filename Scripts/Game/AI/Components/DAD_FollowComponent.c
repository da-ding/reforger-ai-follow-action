[ComponentEditorProps(category: "GameScripted/AI", description: "Component to orchestrate waypoints for following")]
class DAD_FollowComponentClass: ScriptComponentClass
{
};

class DAD_FollowComponent: ScriptComponent
{
	bool m_fixedSpeedBug = false;

	const ResourceName m_WaypointType = "{C37ABB3DCAE43B36}Prefabs/AI/Waypoints/AIWaypoint_FollowFast.et";
	
	
	SCR_ChimeraCharacter m_User = null;
	
	void Follow(SCR_ChimeraCharacter char)
	{
		RplComponent rplC = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rplC.IsOwner()) return;


		Resource wpRes = Resource.Load(m_WaypointType);
		SCR_EntityWaypoint followWaypoint = SCR_EntityWaypoint.Cast(SpawnHelpers.SpawnEntity(wpRes, char.GetOrigin()));

		AIGroup ai = GetAI();

		if (IsFollowing())
		{
			StopFollowing();
		}

		m_User = SCR_ChimeraCharacter.Cast(char);
		followWaypoint.SetEntity(m_User);

		ai.AddWaypointAt(followWaypoint, 0);
		Update();
	}
	
	bool IsFollowing()
	{
		bool isFollowing;
		SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());

		AIWaypoint wp = GetAI().GetCurrentWaypoint();
		if (!wp || !SCR_AIDamageHandling.IsAlive(m_User))
		{
			isFollowing = false;	
		}
		else
		{
			EntityPrefabData prefab = wp.GetPrefabData();
			isFollowing = prefab && prefab.GetPrefabName().Contains("Waypoint_Follow");
		}
		return isFollowing;
	}
	
	void StopFollowing()
	{
		if (!IsFollowing())
		{
			return;
		}
		GetAI().RemoveWaypointAt(0);
		m_User = null;
		UpdateIsFollowing();
	}
	
	// TODO: Get rid of this
	void UpdateIsFollowing()
	{
		AIGroup ai = GetAI();
		bool isFollowing;
		
		AIWaypoint wp = ai.GetCurrentWaypoint();
		if (!wp)
		{
			isFollowing = false;	
		}
		else
		{
			EntityPrefabData prefab = wp.GetPrefabData();
			isFollowing = prefab && prefab.GetPrefabName().Contains("Waypoint_Follow");
		}

		ref array<AIAgent> agents = {};
		ai.GetAgents(agents);
		foreach (AIAgent agent : agents)
		{
			SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
			char.SetIsFollowing(isFollowing);
		}
	}
	
	AIGroup GetAI()
	{
		AIGroup ai = SCR_AIGroup.Cast(GetOwner());

		if (!ai)
		{
			Print("DAD_FollowAction: Could not find AI Group", LogLevel.ERROR);
			return null;
		}
		return ai;
	}

	void Update() {
		Print("DAD_FollowComponent: Updating");
				
		UpdateIsFollowing();

		if (!IsFollowing()) return;
			
		if (!SCR_AIDamageHandling.IsAlive(m_User))
		{
			Print("DAD_FollowComponent: Player is not alive");
			StopFollowing();
			return;
		}


		GetGame().GetCallqueue().CallLater(Update, 2 * 1000, false);

		Print("Following!");		
		AIGroup ai = GetAI();
		if (!ai) return;

		vector playerOrigin = m_User.GetOrigin();

		ref array<AIAgent> agents = {};
		ai.GetAgents(agents);
		

		if (m_User.IsInVehicle() && !ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains("Waypoint_GetIn"))
		{

			foreach (AIAgent a: agents)
			{
				
				ChimeraCharacter character = ChimeraCharacter.Cast(a.GetControlledEntity());
				if (character && !character.IsInVehicle())
				{
					AIWaypoint getInWaypoint = AIWaypoint.Cast(SpawnHelpers.SpawnEntity(
						Resource.Load("{0A2A37B4A56D74DF}PrefabsEditable/Auto/AI/Waypoints/E_AIWaypoint_GetInNearest.et"),
						playerOrigin
					));
					AIWaypoint stayInCar = AIWaypoint.Cast(SpawnHelpers.SpawnEntity(
						Resource.Load("{90E783A161383314}PrefabsEditable/Auto/AI/Waypoints/E_AIWaypoint_Wait.et"),
						playerOrigin
					));
					ai.AddWaypointAt(stayInCar, 0);
					ai.AddWaypointAt(getInWaypoint, 0);
					break;
				}
			}
		}
		if (!m_User.IsInVehicle())
		{
			while (ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains("Waypoint_GetIn"))
			{
				ai.RemoveWaypointAt(0);
			}
			
			while (ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains("Waypoint_Wait"))
			{
				ai.RemoveWaypointAt(0);
			}
		}

		
		if (m_fixedSpeedBug) return;
		Print("Uh Oh! There's a bug here!");

		float distance = vector.Distance(GetOwner().GetOrigin(), playerOrigin);

		if (distance < 7) return;
		
		Print("Okay, Fix time!!");

		AIWaypoint moveWaypoint = AIWaypoint.Cast(SpawnHelpers.SpawnEntity(
				Resource.Load("{06E1B6EBD480C6E0}Prefabs/AI/Waypoints/AIWaypoint_ForcedMove.et"),
				playerOrigin
		));

		moveWaypoint.SetOrigin(playerOrigin);
		ai.AddWaypointAt(moveWaypoint, 0);
		Print("Added waypoint to ");
		Print(playerOrigin);
		Print(ai.GetCurrentWaypoint());
		m_fixedSpeedBug = true;
	}
}
