

modded class SCR_ChimeraCharacterClass
{
};

modded class SCR_ChimeraCharacter
{
	[RplProp()]
	private bool m_DAD_IsFollowing = false;

	bool GetIsFollowing() { return m_DAD_IsFollowing; }

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void SetIsFollowing(bool isFollowing)
	{
		if (m_DAD_IsFollowing == isFollowing) return;
		m_DAD_IsFollowing = isFollowing;
		Replication.BumpMe();
	}
};
