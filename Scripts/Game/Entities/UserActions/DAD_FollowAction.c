class DAD_FollowAction : ScriptedUserAction {

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent rplC = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rplC.IsOwner()) return;

		AIControlComponent controlComp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		AIGroup ai = controlComp.GetControlAIAgent().GetParentGroup();
		DAD_FollowComponent followC = DAD_FollowComponent.Cast(ai.FindComponent(DAD_FollowComponent));

		if (IsFollowing())
		{
			followC.StopFollowing();
		}
		else
		{
			followC.Follow(SCR_ChimeraCharacter.Cast(pUserEntity));
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

	//[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool IsFollowing()
	{
		SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());
		return owner.GetIsFollowing();
	}
}